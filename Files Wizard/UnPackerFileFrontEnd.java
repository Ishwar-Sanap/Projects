import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class UnPackerFileFrontEnd extends Template implements ActionListener
{
    JButton SUBMIT ,PREVIOUS,chooseFileBtn ,choosedirBtn;
    JLabel label1 , label2 ,title;
    JTextField text1 ,text2;
    String dirNameToExtract = "";
    String combinedFile = "";
    
   public UnPackerFileFrontEnd()
    {
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        title = new JLabel("Unpacking Portal");
        Dimension size = title.getPreferredSize();
        title.setBounds(40, 30, size.width+120, size.height+20);
        title.setFont(new Font("Century", Font.BOLD, 20));
        title.setForeground(Color.red);

        label1 = new JLabel();
        label1.setText("Select the file to extract");
        label1.setForeground(Color.white);
        label1.setBounds(40, 40, 200, 20);

        chooseFileBtn = new JButton("Choose file");
        Dimension cd = chooseFileBtn.getPreferredSize();
        chooseFileBtn.setBounds(40,80,150 , 20);

        label2 = new JLabel();
        label2.setText("Select the location to extract the files");
        label2.setForeground(Color.white);
        label2.setBounds(40, 120, 250, 20);

        choosedirBtn = new JButton("Choose directory");
        Dimension cdb = choosedirBtn.getPreferredSize();
        choosedirBtn.setBounds(40, 160, 150, 20);        

        SUBMIT = new JButton("Extract");
        Dimension bsize = SUBMIT.getPreferredSize();
        SUBMIT.setBounds(40,200,bsize.width+20,bsize.height);
        
        PREVIOUS = new JButton("PREVIOUS");
        Dimension b2size = PREVIOUS.getPreferredSize();
        PREVIOUS.setBounds(160, 200, b2size.width+20, b2size.height);
        
        _header.add(title);
        _content.add(label1);
        _content.add(chooseFileBtn);
        _content.add(label2);
        _content.add(choosedirBtn);
        _content.add(SUBMIT);
        _content.add(PREVIOUS);

        chooseFileBtn.addActionListener(this); 
        choosedirBtn.addActionListener(this);
        SUBMIT.addActionListener(this);
        PREVIOUS.addActionListener(this);

        this.setSize(500,500);
        this.setResizable(false);
        setLocationRelativeTo(null); // place window at center of the srcreen 
        this.setVisible(true);
    }

    public void actionPerformed(ActionEvent ae)
    {
        if(ae.getSource() == chooseFileBtn)
        {
            JFileChooser fileChooser = new JFileChooser();
            int response = fileChooser.showOpenDialog(null);

            if(response == JFileChooser.APPROVE_OPTION)
            {
                chooseFileBtn.setText( fileChooser.getSelectedFile().getName());
                String str = fileChooser.getSelectedFile().getAbsolutePath();

                System.out.println(str);
                combinedFile = str;
            }
        }

        else if(ae.getSource() == choosedirBtn)
        {
            JFileChooser fileChooser = new JFileChooser();
            fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

            int response = fileChooser.showOpenDialog(null);

            if(response == JFileChooser.APPROVE_OPTION)
            {
                choosedirBtn.setText(fileChooser.getSelectedFile().getAbsolutePath());
                String str = fileChooser.getSelectedFile().getAbsolutePath();

                dirNameToExtract = str;
            }
        }

        else if(ae.getSource() == SUBMIT)
        {

            if(dirNameToExtract.length() == 0 || combinedFile.length() == 0)
            {
                JOptionPane.showMessageDialog(this,"Select the directory and \nCombined file name ","Error",JOptionPane.ERROR_MESSAGE);
            }

            else
            {
                UnPackingFiles uf = new UnPackingFiles(combinedFile,dirNameToExtract); // go to backend
    
                if(UnPackingFiles.ret == -1)
                {
                    JOptionPane.showMessageDialog(this,"Invalid File !!","Error",JOptionPane.ERROR_MESSAGE);
                }
                else
                {
    
                    JOptionPane.showMessageDialog(this, "Files are successfully extracted in the\n"+ dirNameToExtract);
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
