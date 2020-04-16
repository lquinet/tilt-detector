package com.test.mike.testappnfc8;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileWriter;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by michael on 29-03-16.
 */
public class NDEFRead extends Activity{
    //private DataDevice dataDevice;

    private NfcAdapter mAdapter;
    private PendingIntent mPendingIntent;
    private IntentFilter[] mFilters;
    private String[][] mTechLists;
    private DataDevice ma = (DataDevice)getApplication();

    String dateNow;
    public static String EXTRA_1= "EXTRA_1";
    private TextView textTypeNdefMessage;
    private TextView textViewNdefMessage;
    private TextView textViewDateMessage;

    String startAddressString = null;
    String endAddressString = null;

    byte[] addressStart = null;
    byte[] addressEnd = null;

    byte[] ReadBlockAnswer1 = null;
    byte[] ReadBlockAnswer2 = null;

    String sNbOfBlock = null;
    byte [] numberOfBlockToRead = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ndef_read);

        mAdapter = NfcAdapter.getDefaultAdapter(this);
        mPendingIntent = PendingIntent.getActivity(this, 0, new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);
        IntentFilter ndef = new IntentFilter(NfcAdapter.ACTION_TAG_DISCOVERED);
        mFilters = new IntentFilter[]{
                ndef,
        };
        mTechLists = new String[][]{new String[]{android.nfc.tech.NfcV.class.getName()}};
        ma = (DataDevice) getApplication();

        textViewNdefMessage = (TextView) findViewById(R.id.textViewNdefMessage);
        textViewDateMessage = (TextView) findViewById(R.id.textViewDateMessage);
        //textTypeNdefMessage = (TextView) findViewById(R.id.text_type_ndef_message);

        Bundle extras = getIntent().getExtras();
        if (extras == null) {
            return;
        }

        String sReadNdef = extras.getString(EXTRA_1);
        Date dNow = new Date();
        SimpleDateFormat ft = new SimpleDateFormat("yyyy.MM.dd 'at' hh:mm:ss");
        dateNow = ft.format(dNow);
        textViewDateMessage.setText(dateNow);

        Log.v(dateNow, "ON RESUME NFC APPS ACTIVITY");
        Log.v(String.valueOf(sReadNdef.length()), "ON RESUME NFC APPS ACTIVITY");

        String strTypeNdefMessage="-";

        if(sReadNdef.length() == 0)
        {
            strTypeNdefMessage = ".";
            Log.v("Pas message", "ON RESUME NFC APPS ACTIVITY");
        }
        else {
            if (sReadNdef.startsWith("type:SMARTPOSTER")) {
                strTypeNdefMessage = "smartposter ";
            }
            if (sReadNdef.startsWith("type:TEXT")) {
                strTypeNdefMessage = "text ";
                Log.v("ndef type TEXT", "ON RESUME NFC APPS ACTIVITY");
            }
            if (sReadNdef.startsWith("type:URI")) {
                strTypeNdefMessage = "uri ";
            }
            if (sReadNdef.startsWith("type:MIME:vcard")) {
                strTypeNdefMessage = "MIME vcard ";
            }
            if (sReadNdef.startsWith("type:MIME:application")) {
                strTypeNdefMessage = "MIME application ";
            }
            if (sReadNdef.startsWith("type:MIME:text")) {
                strTypeNdefMessage = "MIME text ";
            }
            if (sReadNdef.startsWith("type:MIME:image")) {
                strTypeNdefMessage = "MIME image ";
            }
            if (sReadNdef.startsWith("type:MIME:audio")) {
                strTypeNdefMessage = "MIME audio ";
            }
            if (sReadNdef.startsWith("type:MIME:video")) {
                strTypeNdefMessage = "MIME video ";
            }
            if (sReadNdef.startsWith("type:MIME:other")) {
                strTypeNdefMessage = "MIME ";
            }
            if (sReadNdef.startsWith("type:EXTERNAL")) {
                strTypeNdefMessage = "EXTERNAL type ";
            }
        }
        //textTypeNdefMessage.setText(strTypeNdefMessage);

        android.text.util.Linkify.addLinks(textViewNdefMessage, 1);
        Log.v("message ",sReadNdef);

        textViewNdefMessage.setText(sReadNdef);

        Log.v("Pas message 2", "ON RESUME NFC APPS ACTIVITY");

/*    startAddressString = "007E";//2043
        addressStart = Helper.ConvertStringToHexBytes(startAddressString);

        endAddressString = "007F";//2044
        addressEnd = Helper.ConvertStringToHexBytes(endAddressString);

        //numberOfBlockToRead = Helper.ConvertStringToHexBytes(sNbOfBlock);
        //sNbOfBlock="1";
        //numberOfBlockToRead = Helper.ConvertIntTo2bytesHexaFormat(Integer.parseInt(sNbOfBlock));

        //Log.v("Nbr Block", String.valueOf(numberOfBlockToRead));

        ReadBlockAnswer1 = NFCCommand.Send_several_ReadSingleBlockCommands (ma.getCurrentTag(),addressStart,numberOfBlockToRead,ma);
        ReadBlockAnswer2 = NFCCommand.Send_several_ReadSingleBlockCommands (ma.getCurrentTag(),addressEnd,numberOfBlockToRead,ma);


        //ReadBlockAnswer1 = NFCCommand.SendReadMultipleBlockCommandCustom(ma.getCurrentTag(),addressStart,numberOfBlockToRead[1],ma);
        //ReadBlockAnswer2 = NFCCommand.SendReadMultipleBlockCommandCustom(ma.getCurrentTag(),addressEnd,numberOfBlockToRead[1],ma);

        //ReadBlockAnswer1 = NFCCommand.SendReadSingleBlockCommand(ma.getCurrentTag(),addressStart,ma);
        //ReadBlockAnswer2 = NFCCommand.SendReadSingleBlockCommand(ma.getCurrentTag(),addressEnd,ma);

        //Log.v("Readblock 1", String.valueOf(ReadBlockAnswer1));

        String hour = Helper.ConvertHexByteToString(ReadBlockAnswer1[0]).toUpperCase();
        String minute = Helper.ConvertHexByteToString(ReadBlockAnswer2[0]).toUpperCase();

        //int hour = Helper.ConvertStringToInt(Helper.ConvertHexByteToString(ReadBlockAnswer1[0]).toUpperCase());
        //int minute = Helper.ConvertStringToInt(Helper.ConvertHexByteToString(ReadBlockAnswer2[0]).toUpperCase());

        Log.v("Hour", hour);
        Log.v("Minute", minute);
        *///boolean stat = saveTagToFile(sReadNdef,dateNow,textTypeNdefMessage.getText().toString());
    }

    public boolean saveTagToFile(String message, String date, String type)
    {
        boolean status = true;
        String  FILENAME = "NDEFMessages.txt";
        String string = "<m>"+message+"<d>"+date+"<t>"+type+"<end>";

        Log.v("message à écrire ",string);


        try
        {
            File folder = new File(Environment.getExternalStorageDirectory(), "NfcV-Reader");
            if (!folder.exists())
                folder.mkdir();

            File file = new File(folder, FILENAME);
            if (!file.exists())
                file.createNewFile();

            FileWriter fos = new FileWriter(file,true);
            fos.append(string);
            fos.close();
        }

        catch (Exception e)
        {
            e.printStackTrace();
            Toast.makeText(this, "impossible d'ecrire",Toast.LENGTH_SHORT).show();
            return false;
        }

        return status;
    }

    @Override
    protected void onNewIntent(Intent intent)
    {
        // TODO Auto-generated method stub
        super.onNewIntent(intent);
        Tag tagFromIntent = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);
        DataDevice ma = (DataDevice)getApplication();
        ma.setCurrentTag(tagFromIntent);
        Log.v("NewIntent", "ON RESUME NFC APPS ACTIVITY");
    }

    @Override
    protected void onResume()
    {
        // TODO Auto-generated method stub
        super.onResume();
        mAdapter.enableForegroundDispatch(this, mPendingIntent, mFilters, mTechLists);
    }
}
