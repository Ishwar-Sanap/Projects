import javax.swing.*;
import java.awt.*;

public class Template extends JFrame {
    JPanel _header;
    JPanel _content;
    JPanel _top;

    // Constructor
    public Template() {
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        GridBagLayout grid = new GridBagLayout();
        setLayout(grid);

        _top = new JPanel();
        _top.setBackground(Color.LIGHT_GRAY);
        _top.setLayout(null);

        getContentPane().add(_top, new GridBagConstraints(0, 0, 1, 1, 1, 5, GridBagConstraints.BASELINE,
                GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));

        _header = new JPanel();
        _header.setLayout(null);
        _header.setBackground(Color.white);

        getContentPane().add(_header, new GridBagConstraints(0, 1, 1, 1, 1, 20, GridBagConstraints.BASELINE,
                GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));

        _content = new JPanel();
        _content.setLayout(null);
        _content.setBackground(new Color(0, 50, 120));
        JScrollPane jsp = new JScrollPane(_content, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        jsp.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);

        getContentPane().add(jsp, new GridBagConstraints(0, 2, 1, 1, 1, 75, GridBagConstraints.BASELINE,
                GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));

        setTitle("\\ File Packer - Unpacker \\");
        setResizable(false);
    }

}
