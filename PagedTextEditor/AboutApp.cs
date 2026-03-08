// Copyright (c) 2026, Missing Planet LLC. All rights reserved.
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;

namespace PagedTextEditor
{
    public class AboutForm : Form
    {
        public AboutForm()
        {
            InitializeComponent();
            WindowHelper.EnableDarkTitleBar(this);
        }

        private void InitializeComponent()
        {
            var version = Assembly.GetExecutingAssembly().GetName().Version;
            var versionString = version != null ? $"v{version.Major}.{version.Minor}.{version.Build}" : "v1.0.0";

            Text = "About";
            Size = new Size(340, 220);
            MinimumSize = Size;
            MaximumSize = Size;  // fixed size — no resizing
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            StartPosition = FormStartPosition.CenterParent;
            BackColor = Color.FromArgb(30, 30, 30);

            // --- Icon ---
            var iconBox = new PictureBox
            {
                Image = new Icon("Resources/appicon.ico", 64, 64).ToBitmap(),
                SizeMode = PictureBoxSizeMode.StretchImage,
                Size = new Size(64, 64),
                Location = new Point((ClientSize.Width - 64) / 2, 24),
            };

            // --- App name ---
            var nameLabel = new Label
            {
                Text = "Missing Planet Journal Editor",
                ForeColor = Color.White,
                BackColor = Color.Transparent,
                Font = new Font("Segoe UI", 12f, FontStyle.Bold),
                AutoSize = false,
                Width = ClientSize.Width,
                Height = 28,
                Location = new Point(0, iconBox.Bottom + 14),
                TextAlign = ContentAlignment.MiddleCenter,
            };

            // --- Version ---
            var versionLabel = new Label
            {
                Text = versionString,
                ForeColor = Color.FromArgb(160, 160, 165),
                BackColor = Color.Transparent,
                Font = new Font("Segoe UI", 9f),
                AutoSize = false,
                Width = ClientSize.Width,
                Height = 20,
                Location = new Point(0, nameLabel.Bottom + 4),
                TextAlign = ContentAlignment.MiddleCenter,
            };

            // --- Copyright ---
            var copyrightLabel = new Label
            {
                Text = "© 2026 Missing Planet LLC. All rights reserved.",
                ForeColor = Color.FromArgb(120, 120, 125),
                BackColor = Color.Transparent,
                Font = new Font("Segoe UI", 8f),
                AutoSize = false,
                Width = ClientSize.Width,
                Height = 18,
                Location = new Point(0, versionLabel.Bottom + 4),
                TextAlign = ContentAlignment.MiddleCenter,
            };

            // --- Close button ---
            var closeButton = new Button
            {
                Text = "Close",
                Size = new Size(80, 28),
                FlatStyle = FlatStyle.Flat,
                BackColor = Color.FromArgb(62, 62, 66),
                ForeColor = Color.White,
                Font = new Font("Segoe UI", 9f),
                Cursor = Cursors.Hand,
                DialogResult = DialogResult.OK,
            };
            closeButton.FlatAppearance.BorderColor = Color.FromArgb(80, 80, 85);
            closeButton.Location = new Point((ClientSize.Width - closeButton.Width) / 2, copyrightLabel.Bottom + 12);

            Controls.AddRange(new Control[] { iconBox, nameLabel, versionLabel, copyrightLabel, closeButton });
            AcceptButton = closeButton;
        }
    }
}