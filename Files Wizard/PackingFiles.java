import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.Scanner;

public class PackingFiles 
{
    public static ArrayList<String> al = new ArrayList<>();
    public static int ret;

    public PackingFiles(String dirName , String cFName)
    {
        ret = 0;

        al.add("txt");
        al.add("docx");
        al.add("java");
        al.add("cpp");
        al.add("c");
        al.add("py");
        al.add("html");

        combineTheData(dirName,cFName);
    }

    public static void ModifyTheFile(byte arr[] , int len)
    {
        byte key = 0x0F;
        for(int i = 0; i<len; i++)
        {
            arr[i] = (byte)(arr[i] ^ key);
        }
    }

    public static String getExtension(String fname)
    {
        String extension ="";

        int lasOccur = fname.lastIndexOf(".");
        if(lasOccur>0)
            extension = fname.substring(lasOccur+1);

        return extension;
    }
    public static void combineTheData(String dirName , String cFName) 
    {
        Scanner sobj = new Scanner(System.in);
                
        File fobj = new File(dirName);
        File allFiles[] = fobj.listFiles();

        if(!fobj.exists())
        {
            System.out.println("Directory not exist ");
            sobj.close();
            return;
        }

        try
        {

            // System.out.println("Enter Combined File Name : ");
            // String cFName = sobj.nextLine(); 

            for(int i = 0; i<allFiles.length ; i++)
            {
                File currFileObj = allFiles[i];

                String fileExtension = getExtension(currFileObj.getName());

                if(al.contains(fileExtension) == false )
                {
                    ret = -1;
                    break;
                }
                String fname = currFileObj.getName() + "\t" + currFileObj.length();
                
                //Adding the 100 bytes header which contains fileName and it's size
                for(int j = fname.length(); j<100; j++)
                {
                    fname += " ";
                }

                String pathToCreate = dirName +File.separator+cFName;

                File cfobj = new File(pathToCreate);
                cfobj.createNewFile();
                
                FileOutputStream fo = new FileOutputStream(cfobj, true); //writing 
                FileInputStream fi = new FileInputStream(currFileObj); // Reading
                
                 // For Uniquley Identifying the combine file created by using project add some name to it.
                if(i == 0)
                {
                    byte arr[] = "FileWizard ".getBytes();
                    fo.write(arr,0,arr.length);
                }

                // writing the name and length of the file into the combined file
                byte fnameBytearr[] = fname.getBytes();
                fo.write(fnameBytearr,0,fnameBytearr.length); 


                //writing the data of file into combined file
                byte buffer[] = new byte[100];
                int ret = 0;

                while((ret = fi.read(buffer) ) != -1)
                {
                    // ModifyTheFile(buffer,ret);
                    fo.write(buffer,0,ret);
                }
                
                fo.write('\n'); // writing new line for seperating the each file data
                
                fi.close();
                fo.close();
            }
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
        }
        sobj.close();

    }
}
