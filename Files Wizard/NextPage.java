import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class NextPage  extends Template implements ActionListener
{   
    JLabel label;
    JButton pack,unpack;

    public NextPage()
    {
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        label = new JLabel("WELCOME: ");
        Dimension size = label.getPreferredSize();
        label.setBounds(40,50,size.width + 80, size.height);
        label.setFont(new Font("Century",Font.BOLD,20));
        label.setForeground(Color.red);

        pack = new JButton("Pack Files");
        Dimension bsize =pack.getPreferredSize();
        pack.setBounds(100, 100, bsize.width, bsize.height);
        pack.addActionListener(this);

        unpack = new JButton("Unpack Files");
        Dimension b2size = unpack.getPreferredSize();
        unpack.setBounds(300, 100, b2size.width+10, b2size.height);
        unpack.addActionListener(this);

        _header.add(label);
        _content.add(pack);
        _content.add(unpack);

        this.setSize(500,500);
        setLocationRelativeTo(null);
        this.setVisible(true);
    }

    public void actionPerformed(ActionEvent ae)
    {
        if(ae.getSource() == pack )
        {
            // opening the packing page
            this.setVisible(false);
            try
            {
                PackerFileFrontEnd pff = new PackerFileFrontEnd();
            }
            catch(Exception e)
            {
                System.out.println(e);
            }
        }

        else if(ae.getSource() == unpack)
        {
            // opening the unpacking page
            this.setVisible(false);
            try
            {
                UnPackerFileFrontEnd uff = new UnPackerFileFrontEnd();
            }
            catch(Exception e)
            {
                System.out.println(e);
            }
        }
    }
}
