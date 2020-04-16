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
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.util.Calendar;

/**
 * Created by michael on 20-04-16.
 */
public class Config extends Activity {

    private NfcAdapter mAdapter;
    private PendingIntent mPendingIntent;
    private IntentFilter[] mFilters;
    private String[][] mTechLists;

    private DataDevice ma = (DataDevice)getApplication();

    private byte [] addressStart = null;
    private byte [] addressSecond = null;
    private byte [] addressThird = null;
    private byte[] dataToWrite = new byte[4];
    private byte[] dataToWrite2 = new byte[4];
    private byte[] dataToWrite3 = new byte[4];
    //private DataDevice dataDevice;

    private long cpt = 0;

    byte[] GetSystemInfoAnswer = null;
    private byte[] WriteSingleBlockAnswer = null;

    private Button ApplyButton;

    EditText value1;
    EditText value2;
    EditText value3;

    private static String GET_BLOCK_NAME = "blockname";
    private static String GET_BLOCK_VALUE = "blockvalue";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.config);

        mAdapter = NfcAdapter.getDefaultAdapter(this);
        mPendingIntent = PendingIntent.getActivity(this, 0, new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);
        IntentFilter ndef = new IntentFilter(NfcAdapter.ACTION_TAG_DISCOVERED);
        mFilters = new IntentFilter[] { ndef, };
        mTechLists = new String[][] { new String[] { android.nfc.tech.NfcV.class.getName() } };
        //dataDevice = new DataDevice();
        ma = new DataDevice();

        initListener();

        ApplyButton = (Button) this.findViewById(R.id.btnApply);

        ApplyButton.setOnClickListener(reset);
    }

    View.OnClickListener reset = new View.OnClickListener(){
        public void onClick(View v){
            new StartApplyTask().execute();
        }
    };

    @Override
    protected void onNewIntent(Intent intent)
    {
        // TODO Auto-generated method stub
        super.onNewIntent(intent);
        Tag tagFromIntent = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);
        DataDevice ma = (DataDevice)getApplication();
        ma.setCurrentTag(tagFromIntent);
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

    private void initListener()
    {
        value1 = (EditText) findViewById(R.id.edtth);
        value1.setInputType(InputType.TYPE_CLASS_NUMBER);
        value1.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {
                // TODO Auto-generated method stub
                int astart = value1.getSelectionStart();
                int aend = value1.getSelectionEnd();

                String FieldValue = s.toString().toUpperCase();

                if (Helper.checkDataHexa(FieldValue) == false)
                {
                    value1.setTextKeepState(Helper.checkAndChangeDataHexa(FieldValue));
                    value1.setSelection(astart-1, aend-1);
                }
                else
                    value1.setSelection(astart, aend);
            }

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                // TODO Auto-generated method stub

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // TODO Auto-generated method stub
            }
        });

        value2 = (EditText) findViewById(R.id.edttempmin);
        value2.setInputType(android.text.InputType.TYPE_CLASS_NUMBER);
        value2.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {
                // TODO Auto-generated method stub
                int astart = value2.getSelectionStart();
                int aend = value2.getSelectionEnd();

                String FieldValue = s.toString().toUpperCase();

                if (Helper.checkDataHexa(FieldValue) == false)
                {
                    value2.setTextKeepState(Helper.checkAndChangeDataHexa(FieldValue));
                    value2.setSelection(astart-1, aend-1);
                }
                else
                    value2.setSelection(astart, aend);
            }

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // TODO Auto-generated method stub
            }
        });

        value3 = (EditText) findViewById(R.id.edttempmax);
        value3.setInputType(android.text.InputType.TYPE_CLASS_NUMBER);
        value3.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {
                // TODO Auto-generated method stub
                int astart = value3.getSelectionStart();
                int aend = value3.getSelectionEnd();

                String FieldValue = s.toString().toUpperCase();

                if (Helper.checkDataHexa(FieldValue) == false)
                {
                    value3.setTextKeepState(Helper.checkAndChangeDataHexa(FieldValue));
                    value3.setSelection(astart-1, aend-1);
                }
                else
                    value3.setSelection(astart, aend);
            }

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // TODO Auto-generated method stub
            }
        });
    }

    private class StartApplyTask extends AsyncTask<Void, Void, Void> {
        private final ProgressDialog dialog = new ProgressDialog(Config.this);

        // can use UI thread here
        protected void onPreExecute() {
            this.dialog.setMessage("Please, place your phone near the card");
            this.dialog.show();

            Log.v("write test 1", "ON RESUME NFC APPS ACTIVITY");

            Log.v("write test 2", "ON RESUME NFC APPS ACTIVITY");
            DataDevice dataDevice = (DataDevice)getApplication();

            GetSystemInfoAnswer = NFCCommand.SendGetSystemInfoCommandCustom(dataDevice.getCurrentTag(),dataDevice);

            if(DecodeGetSystemInfoResponse(GetSystemInfoAnswer))
            {
                Log.v("write test 3", "ON RESUME NFC APPS ACTIVITY");
                String startAddressString = "07FA"; //Block number to reset the card (07FA)
                startAddressString = Helper.castHexKeyboard(startAddressString); //Convert String to Hex
                startAddressString = Helper.FormatStringAddressStart(startAddressString, dataDevice);
                addressStart = Helper.ConvertStringToHexBytes(startAddressString); //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                String valueBlock1 = "00";
                String valueBlock2 = "01";
                String valueBlock3 = "FF";
                String valueBlock4 = "FF";

                String valueBlockTotal = "";
                valueBlockTotal += valueBlock1 + valueBlock2;
                byte[] valueBlockWrite = Helper.ConvertStringToHexBytes(valueBlockTotal); //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                dataToWrite[0] = valueBlockWrite[0];
                dataToWrite[1] = valueBlockWrite[1];

                valueBlockTotal = "";
                valueBlockTotal += valueBlock3 + valueBlock4;
                valueBlockWrite = Helper.ConvertStringToHexBytes(valueBlockTotal);  //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                dataToWrite[2] = valueBlockWrite[0];
                dataToWrite[3] = valueBlockWrite[1];

                Log.v("7FA", String.valueOf(valueBlockWrite));

                String secondAddressString = "07FE";//07FE
                secondAddressString = Helper.castHexKeyboard(secondAddressString); //Convert String to Hex
                secondAddressString = Helper.FormatStringAddressStart(secondAddressString, dataDevice);
                addressSecond = Helper.ConvertStringToHexBytes(secondAddressString); //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                String valueBlock5 = "FF";
                String valueBlock6 = "FF";

                float temp3 = ((Float.valueOf(value1.getText().toString()))/63);
                //int temp3 = (Integer.parseInt(value1.getText().toString()))/1000;
                Log.v("tem3", String.valueOf(temp3));

                String valueBlock7 = Helper.ConvertHexByteToString((int)temp3); //Threshold Value

                Log.v("tem3 2", valueBlock7);

                String valueBlock8 = "FF";

                Log.v("TH", String.valueOf(temp3));
                Log.v("TH",valueBlock7);

                valueBlockTotal = "";
                valueBlockTotal += valueBlock5 + valueBlock6;
                valueBlockWrite = Helper.ConvertStringToHexBytes(valueBlockTotal); //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                Log.v("value block tot", valueBlockTotal);

                dataToWrite2[0] = valueBlockWrite[0];
                dataToWrite2[1] = valueBlockWrite[1];

                valueBlockTotal = "";
                valueBlockTotal += valueBlock7 + valueBlock8;

                Log.v("value block tot2", valueBlockTotal);

                valueBlockWrite = Helper.ConvertStringToHexBytes(valueBlockTotal);  //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                dataToWrite2[2] = valueBlockWrite[0];
                dataToWrite2[3] = valueBlockWrite[1];

                //
                String thirdAddressString = "07FF";//07FF
                thirdAddressString = Helper.castHexKeyboard(thirdAddressString); //Convert String to Hex
                thirdAddressString = Helper.FormatStringAddressStart(thirdAddressString, dataDevice);
                addressThird = Helper.ConvertStringToHexBytes(thirdAddressString); //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                byte[] temp = new byte[2];

                //temp = intToByteArray(value2);

                //String temp = value2.getText().toString();
                float number = Float.valueOf(value3.getText().toString());
                Log.v("value3", String.valueOf(number));

                temp = intToByteArray(number);

                String valueBlock9 = Helper.ConvertHexByteToString(temp[1]);
                String valueBlock10 = Helper.ConvertHexByteToString(temp[0]);

                number = Float.valueOf(value2.getText().toString());

                Log.v("value3", String.valueOf(number));

                temp = intToByteArray(number);

                String valueBlock11 = Helper.ConvertHexByteToString(temp[1]);
                String valueBlock12 = Helper.ConvertHexByteToString(temp[0]);

                valueBlockTotal = "";
                valueBlockTotal += valueBlock9 + valueBlock10;
                Log.v("block tot", valueBlockTotal);

                valueBlockWrite = Helper.ConvertStringToHexBytes(valueBlockTotal); //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                dataToWrite3[0] = valueBlockWrite[0];
                dataToWrite3[1] = valueBlockWrite[1];

                valueBlockTotal = "";
                valueBlockTotal += valueBlock11 + valueBlock12;
                valueBlockWrite = Helper.ConvertStringToHexBytes(valueBlockTotal);  //Convert a "4-char String" to a two bytes format. Example : "0F43" -> { 0X0F ; 0X43 }

                dataToWrite3[2] = valueBlockWrite[0];
                dataToWrite3[3] = valueBlockWrite[1];/**/
            }
        }

        // automatically done on worker thread (separate from UI thread)
        @Override
        protected Void doInBackground(Void... params) {

            // TODO Auto-generated method stub
            cpt = 0;
            DataDevice dataDevice = (DataDevice)getApplication();

            WriteSingleBlockAnswer = null;
            if(DecodeGetSystemInfoResponse(GetSystemInfoAnswer))
            {
                Log.v("write test 4", "ON RESUME NFC APPS ACTIVITY");
                while ((WriteSingleBlockAnswer == null || WriteSingleBlockAnswer[0] == 1) && cpt <= 10)
                {
                    Log.v("write test 5", "ON RESUME NFC APPS ACTIVITY");
                    WriteSingleBlockAnswer = NFCCommand.SendWriteSingleBlockCommand(dataDevice.getCurrentTag(), addressStart, dataToWrite, dataDevice);
                    cpt++;
                }
            }
            WriteSingleBlockAnswer = null;
            if(DecodeGetSystemInfoResponse(GetSystemInfoAnswer))
            {
                Log.v("write test 4", "ON RESUME NFC APPS ACTIVITY");
                while ((WriteSingleBlockAnswer == null || WriteSingleBlockAnswer[0] == 1) && cpt <= 10)
                {
                    Log.v("write test 5", "ON RESUME NFC APPS ACTIVITY");
                    WriteSingleBlockAnswer = NFCCommand.SendWriteSingleBlockCommand(dataDevice.getCurrentTag(), addressSecond, dataToWrite2, dataDevice);
                    cpt++;
                }
            }
            WriteSingleBlockAnswer = null;
            if(DecodeGetSystemInfoResponse(GetSystemInfoAnswer))
            {
                Log.v("write test 4", "ON RESUME NFC APPS ACTIVITY");
                while ((WriteSingleBlockAnswer == null || WriteSingleBlockAnswer[0] == 1) && cpt <= 10)
                {
                    Log.v("write test 5", "ON RESUME NFC APPS ACTIVITY");
                    WriteSingleBlockAnswer = NFCCommand.SendWriteSingleBlockCommand(dataDevice.getCurrentTag(), addressThird, dataToWrite3, dataDevice);
                    cpt++;
                }
            }
        return null;
        }

        // can use UI thread here
        protected void onPostExecute(final Void unused)
        {
            Log.v("write test 6", "ON RESUME NFC APPS ACTIVITY");
            if (this.dialog.isShowing())
                this.dialog.dismiss();

            if (WriteSingleBlockAnswer==null)
            {
                Log.v("write test 7", "ON RESUME NFC APPS ACTIVITY");
                Toast.makeText(getApplicationContext(), "ERROR Write (No tag answer) ", Toast.LENGTH_SHORT).show();
            }
            else if(WriteSingleBlockAnswer[0]==(byte)0x01)
            {
                Log.v("write test 8", "ON RESUME NFC APPS ACTIVITY");
                Toast.makeText(getApplicationContext(), "ERROR Write ", Toast.LENGTH_SHORT).show();
            }
            else if(WriteSingleBlockAnswer[0]==(byte)0xFF)
            {
                Log.v("write test 9", "ON RESUME NFC APPS ACTIVITY");
                Toast.makeText(getApplicationContext(), "ERROR Write ", Toast.LENGTH_SHORT).show();
            }
            else if(WriteSingleBlockAnswer[0]==(byte)0x00)
            {
                Log.v("write test 10", "ON RESUME NFC APPS ACTIVITY");
                Toast.makeText(getApplicationContext(), "Write Sucessfull ", Toast.LENGTH_SHORT).show();
                //finish();
            }
            else
            {
                Log.v("write test 11", "ON RESUME NFC APPS ACTIVITY");
                Toast.makeText(getApplicationContext(), "Write ERROR ", Toast.LENGTH_SHORT).show();
            }
        }
    }

    //***********************************************************************/
    //* the function Decode the tag answer for the GetSystemInfo command
    //* the function fills the values (dsfid / afi / memory size / icRef /..)
    //* in the myApplication class. return true if everything is ok.
    //***********************************************************************/
    public boolean DecodeGetSystemInfoResponse (byte[] GetSystemInfoResponse)
    {
        DataDevice ma = (DataDevice)getApplication();
        //if the tag has returned a good response
        if(GetSystemInfoResponse[0] == (byte) 0x00 && GetSystemInfoResponse.length >= 12)
        {
            //DataDevice ma = (DataDevice)getApplication();
            String uidToString = "";
            byte[] uid = new byte[8];
            // change uid format from byteArray to a String
            for (int i = 1; i <= 8; i++)
            {
                uid[i - 1] = GetSystemInfoResponse[10 - i];
                uidToString += Helper.ConvertHexByteToString(uid[i - 1]);
            }

            //***** TECHNO ******
            ma.setUid(uidToString);
            if(uid[0] == (byte) 0xE0)
                ma.setTechno("ISO 15693");
            else if (uid[0] == (byte) 0xD0)
                ma.setTechno("ISO 14443");
            else
                ma.setTechno("Unknown techno");

            //***** MANUFACTURER ****
            if(uid[1]== (byte) 0x02)
                ma.setManufacturer("STMicroelectronics");
            else if(uid[1]== (byte) 0x04)
                ma.setManufacturer("NXP");
            else if(uid[1]== (byte) 0x07)
                ma.setManufacturer("Texas Instruments");
            else if (uid[1] == (byte) 0x01) //MOTOROLA (updated 20140228)
                ma.setManufacturer("Motorola");
            else if (uid[1] == (byte) 0x03) //HITASHI (updated 20140228)
                ma.setManufacturer("Hitachi");
            else if (uid[1] == (byte) 0x04) //NXP SEMICONDUCTORS
                ma.setManufacturer("NXP");
            else if (uid[1] == (byte) 0x05) //INFINEON TECHNOLOGIES (updated 20140228)
                ma.setManufacturer("Infineon");
            else if (uid[1] == (byte) 0x06) //CYLINC (updated 20140228)
                ma.setManufacturer("Cylinc");
            else if (uid[1] == (byte) 0x07) //TEXAS INSTRUMENTS TAG-IT
                ma.setManufacturer("Texas Instruments");
            else if (uid[1] == (byte) 0x08) //FUJITSU LIMITED (updated 20140228)
                ma.setManufacturer("Fujitsu");
            else if (uid[1] == (byte) 0x09) //MATSUSHITA ELECTRIC INDUSTRIAL (updated 20140228)
                ma.setManufacturer("Matsushita");
            else if (uid[1] == (byte) 0x0A) //NEC (updated 20140228)
                ma.setManufacturer("NEC");
            else if (uid[1] == (byte) 0x0B) //OKI ELECTRIC (updated 20140228)
                ma.setManufacturer("Oki");
            else if (uid[1] == (byte) 0x0C) //TOSHIBA (updated 20140228)
                ma.setManufacturer("Toshiba");
            else if (uid[1] == (byte) 0x0D) //MITSUBISHI ELECTRIC (updated 20140228)
                ma.setManufacturer("Mitsubishi");
            else if (uid[1] == (byte) 0x0E) //SAMSUNG ELECTRONICS (updated 20140228)
                ma.setManufacturer("Samsung");
            else if (uid[1] == (byte) 0x0F) //HUYNDAI ELECTRONICS (updated 20140228)
                ma.setManufacturer("Hyundai");
            else if (uid[1] == (byte) 0x10) //LG SEMICONDUCTORS (updated 20140228)
                ma.setManufacturer("LG");
            else
                ma.setManufacturer("Unknown manufacturer");

            if(uid[1]== (byte) 0x02)
            {
                //**** PRODUCT NAME *****
                if(uid[2] >= (byte) 0x04 && uid[2] <= (byte) 0x07)
                {
                    ma.setProductName("LRI512");
                    ma.setMultipleReadSupported(false);
                    ma.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x14 && uid[2] <= (byte) 0x17)
                {
                    ma.setProductName("LRI64");
                    ma.setMultipleReadSupported(false);
                    ma.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x20 && uid[2] <= (byte) 0x23)
                {
                    ma.setProductName("LRI2K");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x28 && uid[2] <= (byte) 0x2B)
                {
                    ma.setProductName("LRIS2K");
                    ma.setMultipleReadSupported(false);
                    ma.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x2C && uid[2] <= (byte) 0x2F)
                {
                    ma.setProductName("M24LR64");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                }
                else if(uid[2] >= (byte) 0x40 && uid[2] <= (byte) 0x43)
                {
                    ma.setProductName("LRI1K");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x44 && uid[2] <= (byte) 0x47)
                {
                    ma.setProductName("LRIS64K");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                }
                else if(uid[2] >= (byte) 0x48 && uid[2] <= (byte) 0x4B)
                {
                    ma.setProductName("M24LR01E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x4C && uid[2] <= (byte) 0x4F)
                {
                    ma.setProductName("M24LR16E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                    if(ma.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0x50 && uid[2] <= (byte) 0x53)
                {
                    ma.setProductName("M24LR02E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(false);
                }
                else if(uid[2] >= (byte) 0x54 && uid[2] <= (byte) 0x57)
                {
                    ma.setProductName("M24LR32E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                    if(ma.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0x58 && uid[2] <= (byte) 0x5B)
                {
                    ma.setProductName("M24LR04E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                }
                else if(uid[2] >= (byte) 0x5C && uid[2] <= (byte) 0x5F)
                {
                    ma.setProductName("M24LR64E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                    if(ma.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0x60 && uid[2] <= (byte) 0x63)
                {
                    ma.setProductName("M24LR08E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                }
                else if(uid[2] >= (byte) 0x64 && uid[2] <= (byte) 0x67)
                {
                    ma.setProductName("M24LR128E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                    if(ma.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0x6C && uid[2] <= (byte) 0x6F)
                {
                    ma.setProductName("M24LR256E");
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                    if(ma.isBasedOnTwoBytesAddress() == false)
                        return false;
                }
                else if(uid[2] >= (byte) 0xF8 && uid[2] <= (byte) 0xFB)
                {
                    ma.setProductName("detected product");
                    ma.setBasedOnTwoBytesAddress(true);
                    ma.setMultipleReadSupported(true);
                    ma.setMemoryExceed2048bytesSize(true);
                }
                else
                {
                    ma.setProductName("Unknown product");
                    ma.setBasedOnTwoBytesAddress(false);
                    ma.setMultipleReadSupported(false);
                    ma.setMemoryExceed2048bytesSize(false);
                }

                //*** DSFID ***
                ma.setDsfid(Helper.ConvertHexByteToString(GetSystemInfoResponse[10]));

                //*** AFI ***
                ma.setAfi(Helper.ConvertHexByteToString(GetSystemInfoResponse[11]));

                //*** MEMORY SIZE ***
                if(ma.isBasedOnTwoBytesAddress())
                {
                    String temp = new String();
                    temp += Helper.ConvertHexByteToString(GetSystemInfoResponse[13]);
                    temp += Helper.ConvertHexByteToString(GetSystemInfoResponse[12]);
                    ma.setMemorySize(temp);
                }
                else
                    ma.setMemorySize(Helper.ConvertHexByteToString(GetSystemInfoResponse[12]));

                //*** BLOCK SIZE ***
                if(ma.isBasedOnTwoBytesAddress())
                    ma.setBlockSize(Helper.ConvertHexByteToString(GetSystemInfoResponse[14]));
                else
                    ma.setBlockSize(Helper.ConvertHexByteToString(GetSystemInfoResponse[13]));

                //*** IC REFERENCE ***
                if(ma.isBasedOnTwoBytesAddress())
                    ma.setIcReference(Helper.ConvertHexByteToString(GetSystemInfoResponse[15]));
                else
                    ma.setIcReference(Helper.ConvertHexByteToString(GetSystemInfoResponse[14]));
            }
            else
            {
                ma.setProductName("Unknown product");
                ma.setBasedOnTwoBytesAddress(false);
                ma.setMultipleReadSupported(false);
                ma.setMemoryExceed2048bytesSize(false);
                //ma.setAfi("00 ");
                ma.setAfi(Helper.ConvertHexByteToString(GetSystemInfoResponse[11]));				//changed 22-10-2014
                //ma.setDsfid("00 ");
                ma.setDsfid(Helper.ConvertHexByteToString(GetSystemInfoResponse[10]));				//changed 22-10-2014
                //ma.setMemorySize("FF ");
                ma.setMemorySize(Helper.ConvertHexByteToString(GetSystemInfoResponse[12]));		//changed 22-10-2014
                //ma.setBlockSize("03 ");
                ma.setBlockSize(Helper.ConvertHexByteToString(GetSystemInfoResponse[13]));			//changed 22-10-2014
                //ma.setIcReference("00 ");
                ma.setIcReference(Helper.ConvertHexByteToString(GetSystemInfoResponse[14]));		//changed 22-10-2014
            }
            return true;
        }

        // in case of Inventory OK and Get System Info HS
        else if (ma.getTechno() == "ISO 15693")
        {
            ma.setProductName("Unknown product");
            ma.setBasedOnTwoBytesAddress(false);
            ma.setMultipleReadSupported(false);
            ma.setMemoryExceed2048bytesSize(false);
            ma.setAfi("00 ");
            ma.setDsfid("00 ");
            ma.setMemorySize("3F ");		//changed 22-10-2014
            ma.setBlockSize("03 ");
            ma.setIcReference("00 ");
            return true;
        }
        //if the tag has returned an error code
        else
            return false;
    }

    /*public static byte[] intToByteArray(float ConvertfloatToByte) //Conversion temp, float to byte
    {
        byte[] ret = new byte[2];
        ret[1] = (byte) ((ConvertfloatToByte/4)/4);
        ret[0] = (byte) (((short)(ConvertfloatToByte /4) & 0x03) << 6);
        return ret;
    }*/

    public static byte[] intToByteArray(float ConvertfloatToByte) //Conversion temp, float to byte
    {
        byte[] ret = new byte[2];
        ret[1] = (byte) (((short)(ConvertfloatToByte*4) & 0xFFFC) /4);
        ret[0] = (byte) (((short)(ConvertfloatToByte*4) & 0x03) << 6);
        return ret;
    }
}