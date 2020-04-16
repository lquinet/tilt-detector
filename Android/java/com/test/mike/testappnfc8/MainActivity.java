package com.test.mike.testappnfc8;

import android.app.Activity;
import android.app.PendingIntent;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity {

    private NfcAdapter mAdapter;
    private PendingIntent mPendingIntent;
    private IntentFilter[] mFilters;
    private String[][] mTechLists;

    private DataDevice dataDevice;

    private Button ScanButton;          //Read Ndef message
    private Button AdvanceButton;
    private Button blockFormatButton;
    private Button uidFormatButton;

    private DataDevice ma = (DataDevice)getApplication();
    private String sNDEFMessage = "nothing";
    private byte[] fullNdefMessage = null;
    private long cpt = 0;
    private long numberOfBlockToRead = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mAdapter = NfcAdapter.getDefaultAdapter(this);
        mPendingIntent = PendingIntent.getActivity(this, 0, new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);
        IntentFilter ndef = new IntentFilter(NfcAdapter.ACTION_TAG_DISCOVERED);
        mFilters = new IntentFilter[] { ndef, };
        mTechLists = new String[][] { new String[] { android.nfc.tech.NfcV.class.getName() } };
        dataDevice = new DataDevice();

        ScanButton = (Button) this.findViewById(R.id.btnScan);
        AdvanceButton = (Button) this.findViewById(R.id.btnAdvance);

        ScanButton.setOnClickListener(scan);
        AdvanceButton.setOnClickListener(advance);
    }

    View.OnClickListener scan = new View.OnClickListener(){
        public void onClick(View v){
            new StartReadTask().execute();
        }
    };

    View.OnClickListener advance = new View.OnClickListener(){
        public void onClick(View v){
            Intent i = new Intent(MainActivity.this, Advance.class);
            startActivity(i);
        }
    };

    @Override
    protected void onNewIntent(Intent intent)
    {
        // TODO Auto-generated method stub
        super.onNewIntent(intent);
        Tag tagFromIntent = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);
        dataDevice.setCurrentTag(tagFromIntent);
    }

    @Override
    protected void onResume()
    {
        // TODO Auto-generated method stub
        super.onResume();
        mAdapter.enableForegroundDispatch(this, mPendingIntent, mFilters, mTechLists);
    }

    @Override
    protected void onPause() {
        cpt = 500;
        super.onPause();
    }

    private class StartReadTask extends AsyncTask<Void, Void, Void> {
        private final ProgressDialog dialog = new ProgressDialog(MainActivity.this);
        // can use UI thread here
        protected void onPreExecute()
        {
            this.dialog.setMessage("Please, keep your phone close to the tag.");
            this.dialog.show();
        }/**/

        @Override
        protected Void doInBackground(Void... params)
        {
            boolean msgIsEmpty = false;
            fullNdefMessage = null;

            //Check Tag detected (memory size, nb byte address ...) by reading System Info
            byte[] GetSystemInfoAnswer = NFCCommand.SendGetSystemInfoCommandCustom(dataDevice.getCurrentTag(),dataDevice);

            if(DecodeGetSystemInfoResponse(GetSystemInfoAnswer))
            {

                byte[] resultBlock0 = new byte[4];
                byte[] resultBlock1 = new byte[12];
                cpt = 0;

                resultBlock0 = null;
                //while ((resultBlock0 == null || resultBlock0[0] == 1)&& cpt<1500)
                while ((resultBlock0 == null || resultBlock0[0] == 1)&& cpt<10)
                {
                    resultBlock0 = NFCCommand.SendReadSingleBlockCommand(dataDevice.getCurrentTag(), new byte[]{0x00,0x00}, dataDevice);
                    cpt ++;
                    //Used for DEBUG : Log.v("CPT ", " CPT Read Block 0 ===> " + String.valueOf(cpt));
                }

                //NDEF format : 4th first bytes of NDEF header
                //CC0 = E1h  =  NDEF message is present
                //CC1 = bit7-6 : Major version
                //		 bit5-4 : Minor version
                //		 bit3-2 : Read access (00:free access)
                //		 bit1-0 : Write access (00:free access / 10:write need password / 11:no write access)
                //CC2 = Memory size of data field (CC2 *8)
                //CC3 = bit7-3 : rfu
                //		 bit2 : IC memory exceed 2040 bytes
                //		 bit1 : rfu
                //		 bit0 : 1=support Multiple read / read single only

                if(resultBlock0[0]==(byte)0x00 && resultBlock0[1]==(byte)0xE1)
                {
                    //NDEF TAG Format valid
                    cpt = 0;
                    resultBlock1 = null;

                    boolean boolMultipleReadCC3=false;
                    //CC3 bit0 analysis for Read Single /Read Multiple option
                    if((resultBlock0[4] & (byte)0x01) == (byte)0x01)
                        boolMultipleReadCC3 = true;
                    else
                        boolMultipleReadCC3 = false;

                    //while ((resultBlock1 == null || resultBlock1[0] == 1)&& cpt<1500)
                    while ((resultBlock1 == null || resultBlock1[0] == 1)&& cpt<10)
                    {
                        //resultBlock1 = NFCCommand.SendReadSingleBlockCommand(dataDevice.getCurrentTag(), new byte[]{0x00,0x01}, dataDevice);
                        resultBlock1 = NFCCommand.Send_several_ReadSingleBlockCommands(dataDevice.getCurrentTag(),new byte[]{0x00,0x01}, new byte[]{0x00,0x0C}, dataDevice);
                        cpt ++;
                        //Used for DEBUG : Log.v("CPT ", " CPT Read Block 0 ===> " + String.valueOf(cpt));
                    }

                    byte[] resultBlockX = new byte[2];
                    int resultBlockY=0;

                    int nbBytesToRead=0;
                    int numberOfBytesToRead=0;

                    //"00" = "Used to Padding"
                    //"01" = "Defines details of the lock bits"
                    //"02" = "Identify reserved memory areas"
                    //"03" = "Contains NDEF message"
                    //"FD" = "Tag proprietary information"
                    //"FE" = "Last TLV block in the data area"

                    if(resultBlock1[0] == (byte)0x00 && resultBlock1[1]==(byte)0x03)  //"03" = "Contains NDEF message"
                    {
                        //LField : if byte1=00 -> message is empty or not well written
                        //Lfield : if byte1=FF -> Lfield coded on 2 bytes (byte2byte3) else byte1
                        if(resultBlock1[2]==(byte)0x00)
                        {
                            msgIsEmpty = true;
                        }
                        else if(resultBlock1[2]==(byte)0xFF)
                        {
                            resultBlockX[0] += (byte)resultBlock1[3];
                            resultBlockX[1] += (byte)resultBlock1[4];
                            numberOfBytesToRead = (Helper.Convert2bytesHexaFormatToInt(resultBlockX));
                            numberOfBytesToRead += 12; //10;
                            if (numberOfBytesToRead % 4 > 0)
                                numberOfBytesToRead += (4-(numberOfBytesToRead % 4)) + 4;
                            resultBlockX = Helper.ConvertIntTo2bytesHexaFormat(numberOfBytesToRead);
                        }
                        else
                        {
                            resultBlockX[0] += (byte)0x00;
                            resultBlockX[1] += (byte)resultBlock1[2];
                            numberOfBytesToRead = (Helper.Convert2bytesHexaFormatToInt(resultBlockX));
                            numberOfBytesToRead += 12; //8;
                            if (numberOfBytesToRead % 4 > 0)
                                numberOfBytesToRead += (4-(numberOfBytesToRead % 4)) + 4;
                            resultBlockX = Helper.ConvertIntTo2bytesHexaFormat(numberOfBytesToRead);
                        }
                    }
                    else if(resultBlock1[0] == (byte)0x00 && resultBlock1[1]==(byte)0x01)  //"01" = "Defines details of the lock bits"
                    {
                        //LField : if byte1=00 -> message is empty or not well written
                        //Lfield : if byte1=FF -> Lfield coded on 2 bytes (byte2byte3) else byte1
                        if(resultBlock1[6]==(byte)0x00)
                        {
                            msgIsEmpty = true;
                        }
                        else if(resultBlock1[6]==(byte)0xFF)
                        {
                            resultBlockX[0] += (byte)resultBlock1[7];
                            resultBlockX[1] += (byte)resultBlock1[8];
                            numberOfBytesToRead = (Helper.Convert2bytesHexaFormatToInt(resultBlockX));
                            numberOfBytesToRead += 12; //10;
                            if (numberOfBytesToRead % 4 > 0)
                                numberOfBytesToRead += (4-(numberOfBytesToRead % 4)) + 4;
                            resultBlockX = Helper.ConvertIntTo2bytesHexaFormat(numberOfBytesToRead);
                        }
                        else
                        {
                            resultBlockX[0] += (byte)0x00;
                            resultBlockX[1] += (byte)resultBlock1[7];
                            numberOfBytesToRead = (Helper.Convert2bytesHexaFormatToInt(resultBlockX));
                            numberOfBytesToRead += 12; //8
                            if (numberOfBytesToRead % 4 > 0)
                                numberOfBytesToRead += (4-(numberOfBytesToRead % 4)) + 4;
                            resultBlockX = Helper.ConvertIntTo2bytesHexaFormat(numberOfBytesToRead) ;
                        }
                    }

                    if (msgIsEmpty == false)
                    {
                        cpt = 0;

                        if(boolMultipleReadCC3 == false || dataDevice.isMultipleReadSupported() == false ) //ex: LRIS2K no Multiple read allowed
                        {
                            while ((fullNdefMessage == null || fullNdefMessage[0] == 1) && cpt < 10 && numberOfBytesToRead != 0)
                            {
                                fullNdefMessage = NFCCommand.Send_several_ReadSingleBlockCommands(dataDevice.getCurrentTag(),new byte[]{0x00,0x00}, resultBlockX, dataDevice);
                                cpt++;
                            }
                        }
                        else
                        {
                            while ((fullNdefMessage == null || fullNdefMessage[0] == 1) && cpt < 10 && numberOfBytesToRead != 0)
                            {
                                fullNdefMessage = NFCCommand.Send_several_ReadMultipleBlockCommands(dataDevice.getCurrentTag(), resultBlockX, dataDevice);
                                //Used for DEBUG : Log.i("NDEF Text", "fullNdefMessage **** : " + Helper.ConvertHexByteArrayToString(fullNdefMessage));
                                cpt++;
                                //Used for DEBUG : Log.i("CPT ", "***** " + String.valueOf(cpt));
                            }
                        }
                    }
                    else
                    {
                        fullNdefMessage = new byte[1];
                        fullNdefMessage[0] = (byte)0x00;
                    }
                }

                else
                {
                    numberOfBlockToRead = 0;   //Not NDEF TAG Format
                }
            }
            return null;
        }

        protected void onPostExecute(final Void unused)
        {
            if (this.dialog.isShowing())
                this.dialog.dismiss();

            if(fullNdefMessage == null)
            {
                Toast toast = Toast.makeText(getApplicationContext(), "Pas d'évènement critique", Toast.LENGTH_SHORT);
                toast.show();
                Log.v("NDEF=null", "ON RESUME NFC APPS ACTIVITY");
                return;
            }
            else if (fullNdefMessage.length == 1 && fullNdefMessage[0] == (byte)0x00)
            {
                Toast toast = Toast.makeText(getApplicationContext(), "NDEF field equal to 0", Toast.LENGTH_SHORT);
                toast.show();
                return;
            }
            else if (fullNdefMessage[0] == (byte)0xAF)
            {
                Toast toast = Toast.makeText(getApplicationContext(), "Error during Read", Toast.LENGTH_SHORT);
                toast.show();
                return;
            }
            else if (fullNdefMessage.length > 1)
            {
                byte[] NdefMessage = new byte[fullNdefMessage.length];
                for(int i =1;i<fullNdefMessage.length;i++)
                {
                    NdefMessage[i-1]= fullNdefMessage[i];
                }
                sNDEFMessage = NDEFMessages.ConvertNDEF_ByteArrayToString_Adapted(NdefMessage);
                Log.v("ndef22", "ON RESUME NFC APPS ACTIVITY");
                Log.v(sNDEFMessage, "ON RESUME NFC APPS ACTIVITY");

                if(sNDEFMessage != "No Ndef Message Found")
                {
                    Intent i = new Intent(MainActivity.this, NDEFRead.class);
                    i.putExtra(NDEFRead.EXTRA_1, sNDEFMessage);
                    startActivity(i);
                }
                else
                {
                    Toast toast = Toast.makeText(getApplicationContext(), "No Tag ?", Toast.LENGTH_SHORT);
                    toast.show();
                }
            }
            else
            {
                sNDEFMessage = "Pas d'évènement critique";
            }
        }
    }

    //***********************************************************************/
    //* the function Decode the tag answer for the GetSystemInfo command
    //* the function fills the values (dsfid / afi / memory size / icRef /..)
    //* in the myApplication class. return true if everything is ok.
    //***********************************************************************/
    public boolean DecodeGetSystemInfoResponse(byte[] GetSystemInfoResponse) {
        //if the tag has returned a god response
        //DataDevice ma = new DataDevice();
        if (GetSystemInfoResponse[0] == (byte) 0x00 && GetSystemInfoResponse.length >= 12) {
            //DataDevice ma = (DataDevice) getApplication();
            String uidToString = "";
            byte[] uid = new byte[8];
            // change uid format from byteArray to a String
            for (int i = 1; i <= 8; i++) {
                uid[i - 1] = GetSystemInfoResponse[10 - i];
                uidToString += Helper.ConvertHexByteToString(uid[i - 1]);
            }
            Log.v(uidToString, "ON RESUME NFC APPS ACTIVITY");
            dataDevice.setUid(uidToString);

            if(uid[0] == (byte) 0xE0)
                dataDevice.setTechno("ISO 15693");
            else if (uid[0] == (byte) 0xD0)
                dataDevice.setTechno("ISO 14443");
            else
                dataDevice.setTechno("Unknown techno");

            //***** MANUFACTURER ****
            if(uid[1]== (byte) 0x02)
                dataDevice.setManufacturer("STMicroelectronics");
            else if(uid[1]== (byte) 0x04)
                dataDevice.setManufacturer("NXP");
            else if(uid[1]== (byte) 0x07)
                dataDevice.setManufacturer("Texas Instruments");
            else if (uid[1] == (byte) 0x01) //MOTOROLA (updated 20140228)
                dataDevice.setManufacturer("Motorola");
            else if (uid[1] == (byte) 0x03) //HITASHI (updated 20140228)
                dataDevice.setManufacturer("Hitachi");
            else if (uid[1] == (byte) 0x04) //NXP SEMICONDUCTORS
                dataDevice.setManufacturer("NXP");
            else if (uid[1] == (byte) 0x05) //INFINEON TECHNOLOGIES (updated 20140228)
                dataDevice.setManufacturer("Infineon");
            else if (uid[1] == (byte) 0x06) //CYLINC (updated 20140228)
                dataDevice.setManufacturer("Cylinc");
            else if (uid[1] == (byte) 0x07) //TEXAS INSTRUMENTS TAG-IT
                dataDevice.setManufacturer("Texas Instruments");
            else if (uid[1] == (byte) 0x08) //FUJITSU LIMITED (updated 20140228)
                dataDevice.setManufacturer("Fujitsu");
            else if (uid[1] == (byte) 0x09) //MATSUSHITA ELECTRIC INDUSTRIAL (updated 20140228)
                dataDevice.setManufacturer("Matsushita");
            else if (uid[1] == (byte) 0x0A) //NEC (updated 20140228)
                dataDevice.setManufacturer("NEC");
            else if (uid[1] == (byte) 0x0B) //OKI ELECTRIC (updated 20140228)
                dataDevice.setManufacturer("Oki");
            else if (uid[1] == (byte) 0x0C) //TOSHIBA (updated 20140228)
                dataDevice.setManufacturer("Toshiba");
            else if (uid[1] == (byte) 0x0D) //MITSUBISHI ELECTRIC (updated 20140228)
                dataDevice.setManufacturer("Mitsubishi");
            else if (uid[1] == (byte) 0x0E) //SAMSUNG ELECTRONICS (updated 20140228)
                dataDevice.setManufacturer("Samsung");
            else if (uid[1] == (byte) 0x0F) //HUYNDAI ELECTRONICS (updated 20140228)
                dataDevice.setManufacturer("Hyundai");
            else if (uid[1] == (byte) 0x10) //LG SEMICONDUCTORS (updated 20140228)
                dataDevice.setManufacturer("LG");
            else
                dataDevice.setManufacturer("Unknown manufacturer");

            if(uid[1]== (byte) 0x02)
            {
                //**** PRODUCT NAME *****
                if(uid[2] >= (byte) 0x04 && uid[2] <= (byte) 0x07)
                {
                    dataDevice.setProductName("LRI512");
                    dataDevice.setMultipleReadSupported(false);
                    dataDevice.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x14 && uid[2] <= (byte) 0x17)
                {
                    dataDevice.setProductName("LRI64");
                    dataDevice.setMultipleReadSupported(false);
                    dataDevice.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x20 && uid[2] <= (byte) 0x23)
                {
                    dataDevice.setProductName("LRI2K");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x28 && uid[2] <= (byte) 0x2B)
                {
                    dataDevice.setProductName("LRIS2K");
                    dataDevice.setMultipleReadSupported(false);
                    dataDevice.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x2C && uid[2] <= (byte) 0x2F)
                {
                    dataDevice.setProductName("M24LR64");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                }
                else if(uid[2] >= (byte) 0x40 && uid[2] <= (byte) 0x43)
                {
                    dataDevice.setProductName("LRI1K");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x44 && uid[2] <= (byte) 0x47)
                {
                    dataDevice.setProductName("LRIS64K");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                }
                else if(uid[2] >= (byte) 0x48 && uid[2] <= (byte) 0x4B)
                {
                    dataDevice.setProductName("M24LR01E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x4C && uid[2] <= (byte) 0x4F)
                {
                    dataDevice.setProductName("M24LR16E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                    if(dataDevice.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0x50 && uid[2] <= (byte) 0x53)
                {
                    dataDevice.setProductName("M24LR02E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x54 && uid[2] <= (byte) 0x57)
                {
                    dataDevice.setProductName("M24LR32E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                    if(dataDevice.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0x58 && uid[2] <= (byte) 0x5B)
                {
                    dataDevice.setProductName("M24LR04E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                }
                else if(uid[2] >= (byte) 0x5C && uid[2] <= (byte) 0x5F)
                {
                    dataDevice.setProductName("M24LR64E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                    if(dataDevice.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0x60 && uid[2] <= (byte) 0x63)
                {
                    dataDevice.setProductName("M24LR08E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                }
                else if(uid[2] >= (byte) 0x64 && uid[2] <= (byte) 0x67)
                {
                    dataDevice.setProductName("M24LR128E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                    if(dataDevice.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0x6C && uid[2] <= (byte) 0x6F)
                {
                    dataDevice.setProductName("M24LR256E");
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                    if(dataDevice.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0xF8 && uid[2] <= (byte) 0xFB)
                {
                    dataDevice.setProductName("detected product");
                    dataDevice.setBasedOnTwoBytesAddress(true);
                    dataDevice.setMultipleReadSupported(true);
                    dataDevice.setMemoryExceed2048bytesSize(true);
                }
                else
                {
                    dataDevice.setProductName("Unknown product");
                    dataDevice.setBasedOnTwoBytesAddress(false);
                    dataDevice.setMultipleReadSupported(false);
                    dataDevice.setMemoryExceed2048bytesSize(false);
                }

                //*** DSFID ***
                dataDevice.setDsfid(Helper.ConvertHexByteToString(GetSystemInfoResponse[10]));

                //*** AFI ***
                dataDevice.setAfi(Helper.ConvertHexByteToString(GetSystemInfoResponse[11]));

                //*** MEMORY SIZE ***
                if(dataDevice.isBasedOnTwoBytesAddress())
                {
                    String temp = new String();
                    temp += Helper.ConvertHexByteToString(GetSystemInfoResponse[13]);
                    temp += Helper.ConvertHexByteToString(GetSystemInfoResponse[12]);
                    dataDevice.setMemorySize(temp);
                }
                else
                    dataDevice.setMemorySize(Helper.ConvertHexByteToString(GetSystemInfoResponse[12]));

                //*** BLOCK SIZE ***
                if(dataDevice.isBasedOnTwoBytesAddress())
                    dataDevice.setBlockSize(Helper.ConvertHexByteToString(GetSystemInfoResponse[14]));
                else
                    dataDevice.setBlockSize(Helper.ConvertHexByteToString(GetSystemInfoResponse[13]));

                //*** IC REFERENCE ***
                if(dataDevice.isBasedOnTwoBytesAddress())
                    dataDevice.setIcReference(Helper.ConvertHexByteToString(GetSystemInfoResponse[15]));
                else
                    dataDevice.setIcReference(Helper.ConvertHexByteToString(GetSystemInfoResponse[14]));
            }
            else
            {
                dataDevice.setProductName("Unknown product");
                dataDevice.setBasedOnTwoBytesAddress(false);
                dataDevice.setMultipleReadSupported(false);
                dataDevice.setMemoryExceed2048bytesSize(false);
                //ma.setAfi("00 ");
                dataDevice.setAfi(Helper.ConvertHexByteToString(GetSystemInfoResponse[11]));				//changed 22-10-2014
                //ma.setDsfid("00 ");
                dataDevice.setDsfid(Helper.ConvertHexByteToString(GetSystemInfoResponse[10]));				//changed 22-10-2014
                //ma.setMemorySize("FF ");
                dataDevice.setMemorySize(Helper.ConvertHexByteToString(GetSystemInfoResponse[12]));		//changed 22-10-2014
                //ma.setBlockSize("03 ");
                dataDevice.setBlockSize(Helper.ConvertHexByteToString(GetSystemInfoResponse[13]));			//changed 22-10-2014
                //ma.setIcReference("00 ");
                dataDevice.setIcReference(Helper.ConvertHexByteToString(GetSystemInfoResponse[14]));		//changed 22-10-2014
            }
            return true;
        }

        // in case of Inventory OK and Get System Info HS
        else if (dataDevice.getTechno() == "ISO 15693")
        {
            dataDevice.setProductName("Unknown product");
            dataDevice.setBasedOnTwoBytesAddress(false);
            dataDevice.setMultipleReadSupported(false);
            dataDevice.setMemoryExceed2048bytesSize(false);
            dataDevice.setAfi("00 ");
            dataDevice.setDsfid("00 ");
            dataDevice.setMemorySize("3F ");				//changed 22-10-2014
            dataDevice.setBlockSize("03 ");
            dataDevice.setIcReference("00 ");
            return true;
        }

        //if the tag has returned an error code
        else
            return false;
    }
}