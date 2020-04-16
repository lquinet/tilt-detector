package com.test.mike.testappnfc8;


import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

/**
 * Created by michael on 29-03-16.
 */
public class IdIdentification extends Activity{
    private DataDevice dataDevice;

    private TextView uidTextView;
    private TextView manufacturerTextView;
    private TextView productNameTextView;
    private TextView technoTextView;
    private TextView dsfidTextView;
    private TextView afiTextView;
    private TextView memorySizeTextView;
    private TextView blockSizeTextView;
    private TextView icReferenceTextView;

    private NfcAdapter mAdapter;
    private PendingIntent mPendingIntent;
    private IntentFilter[] mFilters;
    private String[][] mTechLists;
    DataDevice ma = (DataDevice)getApplication();

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.uid_read);

        uidTextView = (TextView) this.findViewById(R.id.uid);

        mAdapter = NfcAdapter.getDefaultAdapter(this);
        mPendingIntent = PendingIntent.getActivity(this, 0, new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);
        IntentFilter ndef = new IntentFilter(NfcAdapter.ACTION_TAG_DISCOVERED);
        mFilters = new IntentFilter[] {ndef,};
        mTechLists = new String[][] { new String[] { android.nfc.tech.NfcV.class.getName() } };
        //ma = (DataDevice)getApplication();
        //dataDevice = (DataDevice)getApplication();
        dataDevice = new DataDevice();
    }

    protected void onNewIntent(Intent intent) {
        // TODO Auto-generated method stub
        Log.v("Test UID", "ON RESUME NFC APPS ACTIVITY");
        super.onNewIntent(intent);
        String action = intent.getAction();
        if (mAdapter.ACTION_TAG_DISCOVERED.equals(action)) {
            Log.v("Test UID1", "ON RESUME NFC APPS ACTIVITY");
            Tag tagFromIntent = intent.getParcelableExtra(mAdapter.EXTRA_TAG);
            dataDevice.setCurrentTag(tagFromIntent);


            byte[] GetSystemInfoAnswer = NFCCommand.SendGetSystemInfoCommandCustom(tagFromIntent, new DataDevice());

            if (DecodeGetSystemInfoResponse(GetSystemInfoAnswer)) {
                Log.v(dataDevice.getUid(), "ON RESUME NFC APPS ACTIVITY2");

                uidTextView = (TextView) this.findViewById(R.id.uid);

                manufacturerTextView = (TextView) this.findViewById(R.id.manufacturer);
                productNameTextView = (TextView) this.findViewById(R.id.product_name);
                technoTextView = (TextView) this.findViewById(R.id.techno);
                dsfidTextView = (TextView) this.findViewById(R.id.dsfid);
                afiTextView  = (TextView) this.findViewById(R.id.afi);
                memorySizeTextView  = (TextView) this.findViewById(R.id.memory_size);
                blockSizeTextView  = (TextView) this.findViewById(R.id.number_block);
                icReferenceTextView  = (TextView) this.findViewById(R.id.ref);

                uidTextView.setText(dataDevice.getUid().toUpperCase());
                manufacturerTextView.setText(dataDevice.getManufacturer());
                productNameTextView.setText(dataDevice.getProductName());
                technoTextView.setText(dataDevice.getTechno());
                dsfidTextView.setText(dataDevice.getDsfid().toUpperCase());
                afiTextView.setText(dataDevice.getAfi().toUpperCase());

                String sTemp = dataDevice.getMemorySize();
                sTemp = Helper.StringForceDigit(sTemp, 4);
                int iTemp = Helper.ConvertStringToInt(sTemp);
                iTemp++;

                memorySizeTextView.setText("Number of block = " + iTemp);

                sTemp = dataDevice.getBlockSize();
                sTemp = Helper.StringForceDigit(sTemp, 4);
                iTemp = Helper.ConvertStringToInt(sTemp);
                iTemp++;
                sTemp = Helper.ConvertHexByteToString((byte)iTemp);
                blockSizeTextView.setText("Number of byte of one block = " + sTemp);
                icReferenceTextView.setText(dataDevice.getIcReference().toUpperCase());

            } else {
                return;
            }
        }
    }

    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        //Used for DEBUG : Log.v("NFCappsActivity.java", "ON RESUME NFC APPS ACTIVITY");
        mPendingIntent = PendingIntent.getActivity(this, 0, new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);
        //mAdapter.enableForegroundDispatch(this, mPendingIntent, mFilters, mTechLists);
        mAdapter.getDefaultAdapter(this).enableForegroundDispatch(this, mPendingIntent, null, null);
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        //Used for DEBUG : Log.v("NFCappsActivity.java", "ON PAUSE NFC APPS ACTIVITY");
        super.onPause();
        if (mAdapter.getDefaultAdapter(this) != null)
            mAdapter.getDefaultAdapter(this).disableForegroundDispatch(this);
        return;
    }

    public boolean DecodeGetSystemInfoResponse(byte[] GetSystemInfoResponse) {
        //if the tag has returned a god response
        if (GetSystemInfoResponse[0] == (byte) 0x00 && GetSystemInfoResponse.length >= 12) {
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
