import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.Scanner;

public class UnPackingFiles {

    public static int ret;

    public UnPackingFiles(String cFname , String PathToExtract)
    {   
        ret = 0;
        ExtractFiles(cFname,PathToExtract);
    }
    public static void ModifyTheFile(byte arr[] , int len)
    {
        byte key = 0x0F;
        for(int i = 0; i<len; i++)
        {
            arr[i] = (byte)(arr[i] ^ key);
        }
    }
    
    public static void ExtractFiles(String cFName , String PathToExtract)
    {
       
        try
        {
            File cfobj = new File(cFName);

            FileInputStream fi = new FileInputStream(cfobj);

            byte brr[] = new byte[11];
            fi.read(brr,0,11);

            String name = new String(brr);

            if(name.equals("FileWizard ") == false)
            {
                ret = -1;
                return;
            }
        
            byte headerArray [] = new byte[100]; // reading FileName and Length 
            int bytesRead = 0;

            while((bytesRead = fi.read(headerArray , 0 , 100)) != -1)
            {
                String str = new String(headerArray);
                String arr[] = str.split("\t");
                
                String fname = arr[0].trim();
                int size = Integer.parseInt(arr[1].trim());
                
                String pathToCreate = PathToExtract + File.separator + fname;

                File fobj = new File(pathToCreate);
                if(!fobj.createNewFile())
                {
                    System.out.println(fname);
                    System.out.println("Unable to create the file");
                    return;
                }
                
                //Reading data from the combined file of the size
                byte dataBytesarr[] = new byte[size];
                fi.read(dataBytesarr, 0, size);

                // ModifyTheFile(dataBytesarr,size);

                //writing into the newly created file
                FileOutputStream fo = new FileOutputStream(new File(pathToCreate));
                fo.write(dataBytesarr, 0, size);

                fi.skip(1); // skipping the \n  (1 bytes) when all the data of the file is read to go in next line for reading
                fo.close();
               
            }

        }
        catch(Exception e)
        {
            System.out.println(e);
        }

    }
}
