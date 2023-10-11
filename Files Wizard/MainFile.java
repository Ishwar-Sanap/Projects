import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.Dimension;
import javax.swing.JLabel;

class LoginPage extends Template implements ActionListener
{
    JButton SUBMIT;
    JLabel label1,label2,label3,TopLabel;
    final JTextField text1, text2;
    private static int attemp = 3;

    public LoginPage()
    {
        this.setSize(500, 500);
        this.setLocationRelativeTo(null);

        
        TopLabel = new JLabel();
        TopLabel.setHorizontalAlignment(SwingConstants.CENTER);
        TopLabel.setText(" Packer Unpacker  : Login ");
        TopLabel.setForeground(Color.red);

        Dimension topsize = TopLabel.getPreferredSize();

        TopLabel.setBounds(50,40,topsize.width+150,topsize.height+20);
         TopLabel.setFont(new Font("Century", Font.BOLD, 20));
        _header.add(TopLabel);

        
        label1 = new JLabel();
        label1.setText("Username :");
        label1.setForeground(Color.white);

        Dimension size = label1.getPreferredSize();

        label1.setBounds(50, 135, size.width, size.height);
        label1.setHorizontalAlignment(SwingConstants.CENTER);

        text1 = new JTextField(15);
        Dimension tsize = text1.getPreferredSize();

        text1.setBounds(200, 135, tsize.width, tsize.height);
        text1.setToolTipText("Enter username ");

        label2 = new JLabel();
        label2.setText("Password :");
        label2.setBounds(50, 175, size.width, size.height);
        label2.setForeground(Color.white);
        label2.setHorizontalAlignment(SwingConstants.CENTER);

        
        text2 = new JPasswordField(15);
        text2.setBounds(200, 175, tsize.width, tsize.height);
        text2.setToolTipText("Enter Password ");

        text2.addFocusListener(new FocusListener()
        {
            public void focusGained(FocusEvent e)
            {
               
            }
            public void focusLost(FocusEvent e)
            {
                label3.setText("");
            }
        });

        SUBMIT = new JButton("SUBMIT");
        SUBMIT.setHorizontalAlignment(SwingConstants.CENTER);

        Dimension ssize = SUBMIT.getPreferredSize();

        SUBMIT.setBounds(165,220,ssize.width+20,ssize.height);

        label3 =new JLabel();
        label3.setText("");

        Dimension l3size = label3.getPreferredSize();

        label3.setForeground(Color.RED);
        label3.setBounds(50, 250, l3size.width, l3size.height);

        _content.add(label1);
        _content.add(text1);

        _content.add(label2);
        _content.add(text2);

        _content.add(label3);
        _content.add(SUBMIT);

        SUBMIT.addActionListener(this);
    }

    public boolean validate(String Username , String password)
    {
        if( (Username.length()<4) || (password.length()<4)  )
        {
            return false;
        }

        else
            return true;
    }
    public void actionPerformed(ActionEvent ae)
    {
        String value1 = text1.getText(); // username
        String value2 = text2.getText(); // password

        if(ae.getSource() == SUBMIT)
        {
            if(validate(value1,value2) == false)
            {
                text1.setText("");
                text2.setText("");
                JOptionPane.showMessageDialog(this,"Short Username or Password","Packer Unpacker" ,JOptionPane.ERROR_MESSAGE);
            }

           else if(value1.equals("abcd") && value2.equals("1234"))
            {
                this.setVisible(false);
                
                NextPage page = new NextPage();
                page.pack();
                page.setVisible(true);
                page.setSize(500,500);
                this.setVisible(false);
            }

            else
            {
                attemp--;

                if(attemp==0)
                { 
                    JOptionPane.showMessageDialog(this, "Number of attemps finished", " packer unpacker", JOptionPane.ERROR_MESSAGE);
                    this.dispose();
                    System.exit(0);
                }

                else
                {

                    JOptionPane.showMessageDialog(this, "Incorrect Login or password", "Error!!!",JOptionPane.ERROR_MESSAGE);
                }

            }
        }
    }

}

public class MainFile 
{
    public static void main(String args[])    
    {
        LoginPage frame = new LoginPage();
        frame.setVisible(true);
    }
}
