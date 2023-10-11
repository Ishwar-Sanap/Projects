import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;

public class PackerFileFrontEnd extends Template implements ActionListener
{
     JButton SUBMIT ,PREVIOUS,chooseBtn;
    JLabel label1 , label2 ,title;
    JTextField text1 ,text2;
    String dirName = "";

    public PackerFileFrontEnd()
    {
        setDefaultCloseOperation(EXIT_ON_CLOSE);    

        title = new JLabel("Packing Portal");
        Dimension size = title.getPreferredSize();
        title.setBounds(40, 30, size.width+80, size.height+20);
        title.setFont(new Font("Century", Font.BOLD, 20));
        title.setForeground(Color.red);

        label1 = new JLabel();
        label1.setText("Select the directory name");
        label1.setForeground(Color.white);
        label1.setBounds(40, 40, 200, 20);

        chooseBtn = new JButton("Choose Directory");
        Dimension cd = chooseBtn.getPreferredSize();
        chooseBtn.setBounds(40,80,150 , 20);

        label2 = new JLabel();
        label2.setText("Destination file name");
        label2.setForeground(Color.white);
        label2.setBounds(40, 120, size.width+60, 20);

        text2 = new JTextField(15);
        Dimension tsize = text2.getPreferredSize();
        text2.setBounds(40, 160, tsize.width, tsize.height);
        text2.setToolTipText("Enter Destination file name ");

        SUBMIT = new JButton("SUBMIT");
        Dimension bsize = SUBMIT.getPreferredSize();
        SUBMIT.setBounds(40,200,bsize.width+20,bsize.height);
        
        PREVIOUS = new JButton("PREVIOUS");
        Dimension b2size = PREVIOUS.getPreferredSize();
        PREVIOUS.setBounds(160, 200, b2size.width+20, b2size.height);
        
        SUBMIT.addActionListener(this);
        PREVIOUS.addActionListener(this);
        chooseBtn.addActionListener(this);

        _header.add(title);
        _content.add(label1);
        _content.add(chooseBtn);
        _content.add(label2);
        _content.add(text2);
        _content.add(SUBMIT);
        _content.add(PREVIOUS);

        this.setSize(500,500);
        this.setResizable(false);
        setLocationRelativeTo(null); // place window at center of the srcreen 
        this.setVisible(true);
    }

    public void actionPerformed(ActionEvent ae)
    {
        if(ae.getSource() == chooseBtn)
        {
            JFileChooser fileChooser = new JFileChooser();
            fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

            int response = fileChooser.showOpenDialog(null);

            if(response == JFileChooser.APPROVE_OPTION)
            {
                String str = fileChooser.getSelectedFile().getAbsolutePath();
                System.out.println(str);
                chooseBtn.setText(str);
                dirName = str;
            }
        }
        else if(ae.getSource() == SUBMIT)
        {
            // String dirName = chooseBtn.getText();
            String cFname = text2.getText();

            if(dirName.length() == 0 || cFname.length() == 0 )
            {
                JOptionPane.showMessageDialog(this,"Select the directory and \nwrite combined file name ","Error",JOptionPane.ERROR_MESSAGE);
            }
            
            else
            {
                System.out.println("Hello");
               PackingFiles pf = new PackingFiles(dirName,cFname); // go to backend

               if(PackingFiles.ret == -1)
                {
                    JOptionPane.showMessageDialog(this,"Files are unsupported !!","Error",JOptionPane.ERROR_MESSAGE);
                }
               
                else
                {
                    JOptionPane.showMessageDialog(this,"All files data are successfully combined in\n"+dirName+File.separator+cFname);
                }
                
                this.setVisible(false);
                this.dispose();
                NextPage t = new NextPage();
            }

        }
        else if(ae.getSource() == PREVIOUS)
        {
             
            this.setVisible(false);
            this.dispose();
            NextPage t = new NextPage();
    
        }
    }

}
