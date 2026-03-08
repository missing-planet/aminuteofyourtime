// Copyright (c) 2026, Missing Planet LLC. All rights reserved.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace PagedTextEditor
{
    static class WindowHelper
    {
        [System.Runtime.InteropServices.DllImport("dwmapi.dll")]
        private static extern int DwmSetWindowAttribute(IntPtr hwnd, int attr, ref int attrValue, int attrSize);

        public static void EnableDarkTitleBar(Form form)
        {
            int value = 1;
            DwmSetWindowAttribute(form.Handle, 20, ref value, sizeof(int));
        }
    }
    
    public class MainForm : Form
    {
        // Image aspect ratio
        private const float IMAGE_WIDTH = 1200f;
        private const float IMAGE_HEIGHT = 740f;
        private const float IMAGE_ASPECT = IMAGE_WIDTH / IMAGE_HEIGHT;

        // Navigation
        private int currentPage = 0;
        private readonly List<(string top, string bottom)> pages = new();

        // Controls
        private Panel imagePanel = null!;
        private Panel letterboxOuter = null!;
        private Panel letterboxPanel = null!;
        private RichTextBox topTextBox = null!;
        private RichTextBox bottomTextBox = null!;
        private Button leftButton = null!;
        private Button rightButton = null!;
        private Label pageLabel = null!;
        private MenuStrip menuStrip = null!;
        private Panel toolbarPanel = null!;

        // Toolbar controls
        private Button boldButton = null!;
        private Button italicButton = null!;
        private Button underlineButton = null!;
        private Button alignLeftButton = null!;
        private Button alignCenterButton = null!;
        private Button alignRightButton = null!;
        private NumericUpDown fontSizeSpinner = null!;

        // Track which text box was last focused
        private RichTextBox? activeTextBox = null;
        
        // Fonts
        private PrivateFontCollection fontCollection = new();
        private FontFamily? kurlandRegular = null;
        private FontFamily? kurlandItalic = null;

        // Image area tracking
        private Rectangle imageRect;
        
        private float dpiScale = 1f;
        private bool isDirty = false;

        public MainForm()
        {
            InitializeComponent();
            WindowHelper.EnableDarkTitleBar(this);
            pages.Add(("", ""));
            UpdateUI();
        }

        private Font LoadFonts(string path, string italicPath, float size)
        {
            using var g = CreateGraphics();
            float dpi = g.DpiY;
            dpiScale = dpi / 72f;
            float pointSize = size * 72f / g.DpiY;

            fontCollection.AddFontFile(path);
            kurlandRegular = fontCollection.Families.Last();

            fontCollection.AddFontFile(italicPath);
            kurlandItalic = fontCollection.Families.Last();

            return new Font(kurlandRegular, pointSize);
        }

        private void InitializeComponent()
        {
            Icon = new Icon("Resources/appicon.ico");

            Text = "Missing Planet Journal Editor";
            Size = new Size(1280, 720);
            MinimumSize = new Size(600, 400);
            BackColor = Color.FromArgb(30, 30, 30);
            Color menuColor = Color.FromArgb(45, 45, 48);

            // --- Menu Strip ---
            menuStrip = new MenuStrip();
            menuStrip.Renderer = new DarkMenuRenderer();
            menuStrip.BackColor = menuColor;
            menuStrip.ForeColor = Color.White;

            var fileMenu = new ToolStripMenuItem("File");
            fileMenu.ForeColor = Color.White;

            var exportItem = new ToolStripMenuItem("Export to Text File...");
            exportItem.BackColor = menuColor;
            exportItem.ForeColor = Color.White;
            exportItem.Click += ExportToTextFile;
            exportItem.ImageScaling = ToolStripItemImageScaling.None;
            exportItem.Padding = new Padding(4, 0, 4, 0);
            fileMenu.DropDownItems.Add(exportItem);
            
            var openItem = new ToolStripMenuItem("Open Text File...");
            openItem.BackColor = menuColor;
            openItem.ForeColor = Color.White;
            openItem.Click += OpenTextFile;
            openItem.ImageScaling = ToolStripItemImageScaling.None;
            openItem.Padding = new Padding(4, 0, 4, 0);
            fileMenu.DropDownItems.Add(openItem);

            var exitItem = new ToolStripMenuItem("Exit");
            exitItem.BackColor = menuColor;
            exitItem.ForeColor = Color.White;
            exitItem.Click += (s, e) => Close();
            exitItem.ImageScaling = ToolStripItemImageScaling.None;
            exitItem.Padding = new Padding(4, 0, 4, 0);
            fileMenu.DropDownItems.Add(exitItem);
            
            var helpMenu = new ToolStripMenuItem("Help");
            helpMenu.ForeColor = Color.White;

            var aboutItem = new ToolStripMenuItem("About...");
            aboutItem.BackColor = menuColor;
            aboutItem.ForeColor = Color.White;
            aboutItem.ImageScaling = ToolStripItemImageScaling.None;
            aboutItem.Padding = new Padding(4, 0, 4, 0);
            aboutItem.Click += (s, e) => new AboutForm().ShowDialog(this);
            helpMenu.DropDownItems.Add(aboutItem);

            menuStrip.Items.Add(fileMenu);
            menuStrip.Items.Add(helpMenu);
            Controls.Add(menuStrip);
            MainMenuStrip = menuStrip;

            // --- Toolbar ---
            toolbarPanel = new Panel
            {
                BackColor = Color.FromArgb(85, 85, 92),
                Dock = DockStyle.Top,
                Height = 36,
                Padding = new Padding(4, 3, 4, 3),
            };
            Controls.Add(toolbarPanel);

            //boldButton      = CreateToolbarButton("B", FontStyle.Bold,      "Bold");
            italicButton    = CreateToolbarButton("I", FontStyle.Italic,     "Italic");
            underlineButton = CreateToolbarButton("U", FontStyle.Underline,  "Underline");

            //boldButton.Click      += (s, e) => ToggleStyle(FontStyle.Bold);
            italicButton.Click    += (s, e) => ToggleStyle(FontStyle.Italic);
            underlineButton.Click += (s, e) => ToggleStyle(FontStyle.Underline);

            alignLeftButton   = CreateToolbarButton("⬅", null, "Align Left");
            alignCenterButton = CreateToolbarButton("↔", null, "Align Center");
            alignRightButton  = CreateToolbarButton("➡", null, "Align Right");

            alignLeftButton.Click   += (s, e) => SetAlignment(HorizontalAlignment.Left);
            alignCenterButton.Click += (s, e) => SetAlignment(HorizontalAlignment.Center);
            alignRightButton.Click  += (s, e) => SetAlignment(HorizontalAlignment.Right);

            var sizeLabel = new Label
            {
                Text = "Size:",
                ForeColor = Color.White,
                BackColor = Color.Transparent,
                AutoSize = false,
                Width = 34,
                Height = 22,
                TextAlign = ContentAlignment.MiddleRight,
            };

            fontSizeSpinner = new NumericUpDown
            {
                Minimum = 4,
                Maximum = 144,
                Value = 11,
                Width = 52,
                Height = 22,
                BackColor = Color.FromArgb(60, 60, 65),
                ForeColor = Color.White,
                BorderStyle = BorderStyle.FixedSingle,
                TextAlign = HorizontalAlignment.Center,
                DecimalPlaces = 0,
            };
            fontSizeSpinner.ValueChanged += FontSizeSpinner_ValueChanged;

            var flow = new FlowLayoutPanel
            {
                Dock = DockStyle.Fill,
                FlowDirection = FlowDirection.LeftToRight,
                WrapContents = false,
                BackColor = Color.Transparent,
                Padding = new Padding(0),
            };

            void AddToFlow(Control c, int hMargin = 2)
            {
                c.Margin = new Padding(hMargin, 3, hMargin, 3);
                flow.Controls.Add(c);
            }

            //AddToFlow(boldButton);
            AddToFlow(italicButton);
            AddToFlow(underlineButton);

            var sep1 = new Panel { Width = 1, Height = 22, BackColor = Color.FromArgb(80, 80, 85) };
            sep1.Margin = new Padding(6, 5, 6, 5);
            flow.Controls.Add(sep1);

            AddToFlow(alignLeftButton);
            AddToFlow(alignCenterButton);
            AddToFlow(alignRightButton);

            var sep2 = new Panel { Width = 1, Height = 22, BackColor = Color.FromArgb(80, 80, 85) };
            sep2.Margin = new Padding(6, 5, 6, 5);
            flow.Controls.Add(sep2);

            sizeLabel.Margin = new Padding(2, 5, 2, 2);
            flow.Controls.Add(sizeLabel);
            fontSizeSpinner.Margin = new Padding(2, 5, 2, 2);
            flow.Controls.Add(fontSizeSpinner);

            toolbarPanel.Controls.Add(flow);

            // --- Outer scrollable container ---
            letterboxOuter = new Panel
            {
                BackColor = Color.Black,
                Dock = DockStyle.Fill,
                AutoScroll = true,
            };
            Controls.Add(letterboxOuter);
            letterboxOuter.BringToFront();
            menuStrip.BringToFront();
            toolbarPanel.BringToFront();

            // Inner panel
            letterboxPanel = new Panel { BackColor = Color.Black };
            letterboxOuter.Controls.Add(letterboxPanel);

            letterboxOuter.Resize += (s, e) => {
                letterboxPanel.Size = new Size(
                    Math.Max(letterboxOuter.ClientSize.Width,  (int)IMAGE_WIDTH  + 60),
                    Math.Max(letterboxOuter.ClientSize.Height, (int)IMAGE_HEIGHT + 60)
                );
                LayoutControls();
            };

            // --- Image Panel ---
            imagePanel = new Panel { BackColor = Color.FromArgb(240, 235, 220) };
            letterboxPanel.Controls.Add(imagePanel);
            imagePanel.Paint += ImagePanel_Paint;

            var kurlandFont = LoadFonts("Resources/Kurland-Regular.ttf", "Resources/Kurland-Italic.ttf", 45f);

            // --- Text Boxes ---
            topTextBox = new RichTextBox
            {
                Font = kurlandFont,
                BackColor = Color.FromArgb(250, 247, 240),
                ForeColor = Color.FromArgb(30, 30, 30),
                BorderStyle = BorderStyle.None,
                ScrollBars = RichTextBoxScrollBars.None,
                AcceptsTab = true,
                WordWrap = true,
            };
            topTextBox.TextChanged      += SaveCurrentPage_Handler;
            topTextBox.Enter            += (s, e) => { activeTextBox = topTextBox; UpdateToolbarState(); };
            topTextBox.SelectionChanged += (s, e) => UpdateToolbarState();

            bottomTextBox = new RichTextBox
            {
                Font = kurlandFont,
                BackColor = Color.FromArgb(250, 247, 240),
                ForeColor = Color.FromArgb(30, 30, 30),
                BorderStyle = BorderStyle.None,
                ScrollBars = RichTextBoxScrollBars.None,
                AcceptsTab = true,
                WordWrap = true,
            };
            bottomTextBox.TextChanged      += SaveCurrentPage_Handler;
            bottomTextBox.Enter            += (s, e) => { activeTextBox = bottomTextBox; UpdateToolbarState(); };
            bottomTextBox.SelectionChanged += (s, e) => UpdateToolbarState();
            
            topTextBox.HideSelection = false;
            bottomTextBox.HideSelection = false;

            imagePanel.Controls.Add(topTextBox);
            imagePanel.Controls.Add(bottomTextBox);

            // --- Navigation Buttons ---
            leftButton = CreateNavButton("◀");
            leftButton.Click += LeftButton_Click;
            letterboxPanel.Controls.Add(leftButton);

            rightButton = CreateNavButton("▶");
            rightButton.Click += RightButton_Click;
            letterboxPanel.Controls.Add(rightButton);

            // --- Page Label ---
            pageLabel = new Label
            {
                ForeColor = Color.White,
                BackColor = Color.Transparent,
                TextAlign = ContentAlignment.MiddleCenter,
                Font = new Font("Segoe UI", 10f),
                AutoSize = false,
            };
            letterboxPanel.Controls.Add(pageLabel);

            Load += (s, e) => {
                letterboxPanel.Size = new Size(
                    Math.Max(letterboxOuter.ClientSize.Width,  (int)IMAGE_WIDTH  + 60),
                    Math.Max(letterboxOuter.ClientSize.Height, (int)IMAGE_HEIGHT + 60)
                );
                LayoutControls();
                fontSizeSpinner.Value = (decimal)Math.Round(topTextBox.Font.Size);
            };

            FormClosing += (s, e) =>
            {
                if (!ConfirmDiscardChanges())
                {
                    e.Cancel = true;
                }
            };
        }
        
        // Returns false if the user cancelled
        private bool ConfirmDiscardChanges()
        {
            if (!isDirty) return true;
            var result = MessageBox.Show(
                "You have unsaved changes. Export before continuing?",
                "Unsaved Changes",
                MessageBoxButtons.YesNoCancel,
                MessageBoxIcon.Warning);

            if (result == DialogResult.Cancel) return false;
            if (result == DialogResult.Yes) ExportToTextFile(null, EventArgs.Empty);
            return true;
        }

        // --- Toolbar helpers ---

        private Button CreateToolbarButton(string text, FontStyle? style, string tooltip)
        {
            var btn = new Button
            {
                Text = text,
                Width = 26,
                Height = 24,
                Font = new Font("Segoe UI", 9f, style ?? FontStyle.Regular),
                ForeColor = Color.White,
                BackColor = Color.FromArgb(62, 62, 66),
                FlatStyle = FlatStyle.Flat,
                Cursor = Cursors.Hand,
            };
            btn.FlatAppearance.BorderColor = Color.FromArgb(80, 80, 85);
            new ToolTip().SetToolTip(btn, tooltip);
            return btn;
        }
        
        private bool IsSelectionItalic(RichTextBox rtb)
        {
            var f = rtb.SelectionFont ?? rtb.Font;
            return f.Style.HasFlag(FontStyle.Italic)          // synthesized italic
                   || f.Name == kurlandItalic?.Name;    // separate italic file
        }

        private void ToggleStyle(FontStyle style)
        {
            var rtb = activeTextBox;
            if (rtb == null) return;
            var current = rtb.SelectionFont ?? rtb.Font;
            var newStyle = current.Style ^ style;

            FontFamily family;
            if (style == FontStyle.Italic)
                family = IsSelectionItalic(rtb)
                    ? (kurlandItalic  ?? kurlandRegular ?? current.FontFamily)
                    : (kurlandRegular ?? current.FontFamily);
            else
                family = kurlandRegular ?? current.FontFamily;

            rtb.SelectionFont = new Font(family, current.Size, newStyle);
            UpdateToolbarState();
            rtb.Focus();

            isDirty = true;
        }

        private void SetAlignment(HorizontalAlignment align)
        {
            var rtb = activeTextBox;
            if (rtb == null) return;
            rtb.SelectionAlignment = align;
            UpdateToolbarState();
            rtb.Focus();

            isDirty = true;
        }

        private void FontSizeSpinner_ValueChanged(object? sender, EventArgs e)
        {
            var rtb = activeTextBox;
            if (rtb == null) return;
            var current = rtb.SelectionFont ?? rtb.Font;
            FontFamily family;
            FontStyle style = current.Style;
            if (style == FontStyle.Italic)
                family = IsSelectionItalic(rtb)
                    ? (kurlandItalic  ?? kurlandRegular ?? current.FontFamily)
                    : (kurlandRegular ?? current.FontFamily);
            else
                family = kurlandRegular ?? current.FontFamily;
            float pointSize = (float)fontSizeSpinner.Value / dpiScale;
            rtb.SelectionFont = new Font(family, pointSize, current.Style);
            rtb.Focus();
        }

        private void UpdateToolbarState()
        {
            var rtb = activeTextBox;
            if (rtb == null) return;

            var font = rtb.SelectionFont ?? rtb.Font;
            //SetButtonActive(boldButton,      font.Bold);
            SetButtonActive(italicButton,    IsSelectionItalic(rtb));
            SetButtonActive(underlineButton, font.Underline);

            SetButtonActive(alignLeftButton,   rtb.SelectionAlignment == HorizontalAlignment.Left);
            SetButtonActive(alignCenterButton, rtb.SelectionAlignment == HorizontalAlignment.Center);
            SetButtonActive(alignRightButton,  rtb.SelectionAlignment == HorizontalAlignment.Right);

            fontSizeSpinner.ValueChanged -= FontSizeSpinner_ValueChanged;
            fontSizeSpinner.Value = Math.Max(fontSizeSpinner.Minimum,
                                    Math.Min(fontSizeSpinner.Maximum, (decimal)Math.Round(font.Size * dpiScale)));
            fontSizeSpinner.ValueChanged += FontSizeSpinner_ValueChanged;
        }

        private void SetButtonActive(Button btn, bool active)
        {
            btn.BackColor = active ? Color.FromArgb(0, 122, 204) : Color.FromArgb(62, 62, 66);
        }

        // --- Existing methods ---

        private void LimitTextToVisible(RichTextBox? rtb)
        {
            if (rtb == null || rtb.TextLength == 0) return;

            int lastIndex = rtb.TextLength - 1;
            var pos = rtb.GetPositionFromCharIndex(lastIndex);

            if (pos.Y + rtb.Font.Height > rtb.ClientSize.Height)
            {
                int selStart = rtb.SelectionStart;

                while (rtb.TextLength > 0)
                {
                    lastIndex = rtb.TextLength - 1;
                    pos = rtb.GetPositionFromCharIndex(lastIndex);
                    if (pos.Y + rtb.Font.Height <= rtb.ClientSize.Height) break;
                    rtb.Text = rtb.Text.Substring(0, lastIndex);
                }

                rtb.SelectionStart  = Math.Min(selStart, rtb.TextLength);
                rtb.SelectionLength = 0;
            }
        }

        private Button CreateNavButton(string text)
        {
            return new Button
            {
                Text = text,
                Font = new Font("Segoe UI", 14f, FontStyle.Bold),
                ForeColor = Color.White,
                BackColor = Color.FromArgb(70, 70, 80),
                FlatStyle = FlatStyle.Flat,
                Cursor = Cursors.Hand,
                Size = new Size(44, 80),
            };
        }

        private void ImagePanel_Paint(object? sender, PaintEventArgs e)
        {
            var g = e.Graphics;
            int dividerX = imagePanel.Width / 2;
            using var pen = new Pen(Color.FromArgb(180, 160, 120), 2f);
            g.DrawLine(pen, dividerX, 20, dividerX, imagePanel.Height - 20);
            using var borderPen = new Pen(Color.FromArgb(160, 140, 100), 3f);
            g.DrawRectangle(borderPen, 1, 1, imagePanel.Width - 3, imagePanel.Height - 3);
        }

        private void LayoutControls()
        {
            int panelW = letterboxPanel.Width;
            int panelH = letterboxPanel.Height;

            int imgW = (int)IMAGE_WIDTH;
            int imgH = (int)IMAGE_HEIGHT;

            int imgX = (panelW - imgW) / 2;
            int imgY = (panelH - imgH) / 2;
            imageRect = new Rectangle(imgX, imgY, imgW, imgH);

            imagePanel.Bounds = imageRect;

            int padding  = Math.Max(10, imgW / 60);
            int spineGap = Math.Max(4,  imgW / 80);
            int halfW    = imgW / 2;
            int textW    = halfW - padding - spineGap / 2;
            int textH    = imgH - padding * 2;

            topTextBox.Bounds    = new Rectangle(padding,              padding, textW, textH);
            bottomTextBox.Bounds = new Rectangle(halfW + spineGap / 2, padding, textW, textH);

            int btnY      = imgY + (imgH - leftButton.Height) / 2;
            int btnMargin = Math.Max(6, (imgX - leftButton.Width) / 2);

            leftButton.Location  = new Point(imgX - leftButton.Width - btnMargin, btnY);
            rightButton.Location = new Point(imgX + imgW + btnMargin,             btnY);

            pageLabel.Bounds = new Rectangle(imgX, imgY + imgH + 6, imgW, 24);

            imagePanel.Invalidate();
        }

        private void SaveCurrentPage()
        {
            if (currentPage < pages.Count)
                pages[currentPage] = (topTextBox.Text, bottomTextBox.Text);
        }

        private void LoadCurrentPage()
        {
            topTextBox.TextChanged    -= SaveCurrentPage_Handler;
            bottomTextBox.TextChanged -= SaveCurrentPage_Handler;

            var page = pages[currentPage];
            topTextBox.Text    = page.top;
            bottomTextBox.Text = page.bottom;

            topTextBox.TextChanged    += SaveCurrentPage_Handler;
            bottomTextBox.TextChanged += SaveCurrentPage_Handler;
        }

        private void SaveCurrentPage_Handler(object? sender, EventArgs e)
        {
            Debug.Assert(sender != null, "SaveCurrentPage_Handler - Sender is null!");
            LimitTextToVisible(sender as RichTextBox);
            SaveCurrentPage();
            isDirty = true;
        }

        private void UpdateUI()
        {
            pageLabel.Text      = $"Page {currentPage + 1} of {pages.Count}";
            leftButton.Enabled  = currentPage > 0;
            rightButton.Enabled = true;
        }

        private void LeftButton_Click(object? sender, EventArgs e)
        {
            if (currentPage <= 0) return;
            SaveCurrentPage();
            currentPage--;
            LoadCurrentPage();
            UpdateUI();
        }

        private void RightButton_Click(object? sender, EventArgs e)
        {
            SaveCurrentPage();
            currentPage++;
            if (currentPage >= pages.Count)
                pages.Add(("", ""));
            LoadCurrentPage();
            UpdateUI();
        }
        
        private void OpenTextFile(object? sender, EventArgs e)
        {
            if (!ConfirmDiscardChanges()) return;

            using var ofd = new OpenFileDialog
            {
                Title = "Open Exported Text File",
                Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*",
                DefaultExt = "txt",
            };
            if (ofd.ShowDialog() != DialogResult.OK) return;

            try
            {
                var allText = File.ReadAllText(ofd.FileName);

                // Split on the page headers produced by the exporter
                var chunks = System.Text.RegularExpressions.Regex.Split(
                        allText, @"===\s*Page\s+\d+\s*===")
                    .Select(s => s.Trim())
                    .Where(s => !string.IsNullOrEmpty(s))
                    .ToList();

                // Each pair of chunks is one spread (left page, right page)
                pages.Clear();
                for (int i = 0; i < chunks.Count; i += 2)
                {
                    string top    = chunks[i];
                    string bottom = i + 1 < chunks.Count ? chunks[i + 1] : "";
                    pages.Add((top, bottom));
                }

                if (pages.Count == 0) pages.Add(("", ""));
                currentPage = 0;
                LoadCurrentPage();
                isDirty = false;
                UpdateUI();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error reading file:\n{ex.Message}",
                    "Open Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ExportToTextFile(object? sender, EventArgs e)
        {
            SaveCurrentPage();

            using var sfd = new SaveFileDialog
            {
                Title      = "Export Pages to Text File",
                Filter     = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*",
                DefaultExt = "txt",
                FileName   = "pages_export.txt"
            };

            if (sfd.ShowDialog() != DialogResult.OK) return;

            try
            {
                using var writer = new StreamWriter(sfd.FileName);
                for (int i = 0; i < pages.Count; i++)
                {
                    var (top, bottom) = pages[i];
                    bool hasContent = !string.IsNullOrWhiteSpace(top) || !string.IsNullOrWhiteSpace(bottom);
                    if (!hasContent && i == pages.Count - 1 && i > 0) continue;

                    writer.WriteLine($"=== Page {i + 1} ===");
                    writer.WriteLine();

                    if (!string.IsNullOrWhiteSpace(top))
                        writer.WriteLine(top.Trim());

                    writer.WriteLine();
                    writer.WriteLine($"=== Page {i + 2} ===");
                    writer.WriteLine();

                    if (!string.IsNullOrWhiteSpace(bottom))
                        writer.WriteLine(bottom.Trim());

                    writer.WriteLine();
                }

                MessageBox.Show($"Successfully exported {pages.Count} page(s) to:\n{sfd.FileName}",
                    "Export Complete", MessageBoxButtons.OK, MessageBoxIcon.Information);

                isDirty = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error writing file:\n{ex.Message}",
                    "Export Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        protected override void OnResize(EventArgs e) => base.OnResize(e);
    }
}

class DarkMenuRenderer : ToolStripProfessionalRenderer
{
    public DarkMenuRenderer() : base(new DarkColorTable()) { }

    protected override void OnRenderItemText(ToolStripItemTextRenderEventArgs e)
    {
        e.TextColor = Color.White;
        if (e.Item.Owner is ToolStripDropDownMenu)
        {
            e.TextRectangle = new Rectangle(
                4,
                e.TextRectangle.Y,
                e.TextRectangle.Width + e.TextRectangle.X - 4,
                e.TextRectangle.Height);
        }
        base.OnRenderItemText(e);
    }

    protected override void OnRenderImageMargin(ToolStripRenderEventArgs e)
    {
        // suppress image margin column
    }
}

class DarkColorTable : ProfessionalColorTable
{
    public override Color MenuItemSelected                  => Color.FromArgb(0, 122, 204);
    public override Color MenuItemBorder                    => Color.FromArgb(0, 122, 204);
    public override Color MenuItemSelectedGradientBegin     => Color.FromArgb(0, 122, 204);
    public override Color MenuItemSelectedGradientEnd       => Color.FromArgb(0,  70, 117);
    public override Color MenuItemPressedGradientBegin      => Color.FromArgb(0,  99, 163);
    public override Color MenuItemPressedGradientEnd        => Color.FromArgb(0,  99, 163);
    public override Color ToolStripDropDownBackground       => Color.FromArgb(45, 45,  48);
    public override Color ImageMarginGradientBegin          => Color.FromArgb(45, 45,  48);
    public override Color ImageMarginGradientMiddle         => Color.FromArgb(45, 45,  48);
    public override Color ImageMarginGradientEnd            => Color.FromArgb(45, 45,  48);
    public override Color ImageMarginRevealedGradientBegin  => Color.FromArgb(45, 45,  48);
    public override Color ImageMarginRevealedGradientMiddle => Color.FromArgb(45, 45,  48);
    public override Color ImageMarginRevealedGradientEnd    => Color.FromArgb(45, 45,  48);
}