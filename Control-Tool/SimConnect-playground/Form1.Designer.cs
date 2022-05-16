namespace SimConnect_playground
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.button1 = new System.Windows.Forms.Button();
            this.simStatus = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.varBank = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.varPitch = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.varBaro = new System.Windows.Forms.Label();
            this.varKias = new System.Windows.Forms.Label();
            this.varAltitude = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.button5 = new System.Windows.Forms.Button();
            this.button6 = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.button7 = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label18 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.testCmdSendSweep = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.testCmdSweepPeriod = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.testCmdSweepStep = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.testCmdSweepTo = new System.Windows.Forms.NumericUpDown();
            this.testCmdSweepFrom = new System.Windows.Forms.NumericUpDown();
            this.testCmdSendOnce = new System.Windows.Forms.Button();
            this.testCmdValue = new System.Windows.Forms.NumericUpDown();
            this.testCmdName = new System.Windows.Forms.TextBox();
            this.testCmdSweepTimer = new System.Windows.Forms.Timer(this.components);
            this.varDataRate = new System.Windows.Forms.Label();
            this.varErrors = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.radioButton3 = new System.Windows.Forms.RadioButton();
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdSweepPeriod)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdSweepStep)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdSweepTo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdSweepFrom)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdValue)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(12, 41);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(130, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "Connect Flight Simulator";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // simStatus
            // 
            this.simStatus.Location = new System.Drawing.Point(148, 46);
            this.simStatus.Name = "simStatus";
            this.simStatus.Size = new System.Drawing.Size(161, 18);
            this.simStatus.TabIndex = 1;
            this.simStatus.Text = "Not Connected";
            // 
            // timer1
            // 
            this.timer1.Interval = 25;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.varBank);
            this.groupBox1.Controls.Add(this.label19);
            this.groupBox1.Controls.Add(this.varPitch);
            this.groupBox1.Controls.Add(this.label17);
            this.groupBox1.Controls.Add(this.varBaro);
            this.groupBox1.Controls.Add(this.varKias);
            this.groupBox1.Controls.Add(this.varAltitude);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Location = new System.Drawing.Point(12, 83);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(186, 94);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Live Sim Data";
            // 
            // varBank
            // 
            this.varBank.Location = new System.Drawing.Point(121, 71);
            this.varBank.Name = "varBank";
            this.varBank.Size = new System.Drawing.Size(45, 13);
            this.varBank.TabIndex = 9;
            // 
            // label19
            // 
            this.label19.Location = new System.Drawing.Point(6, 71);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(109, 15);
            this.label19.TabIndex = 8;
            this.label19.Text = "Bank angle:";
            // 
            // varPitch
            // 
            this.varPitch.Location = new System.Drawing.Point(121, 57);
            this.varPitch.Name = "varPitch";
            this.varPitch.Size = new System.Drawing.Size(45, 13);
            this.varPitch.TabIndex = 7;
            // 
            // label17
            // 
            this.label17.Location = new System.Drawing.Point(6, 57);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(109, 15);
            this.label17.TabIndex = 6;
            this.label17.Text = "Pitch angle:";
            // 
            // varBaro
            // 
            this.varBaro.Location = new System.Drawing.Point(121, 42);
            this.varBaro.Name = "varBaro";
            this.varBaro.Size = new System.Drawing.Size(45, 13);
            this.varBaro.TabIndex = 5;
            // 
            // varKias
            // 
            this.varKias.Location = new System.Drawing.Point(121, 27);
            this.varKias.Name = "varKias";
            this.varKias.Size = new System.Drawing.Size(45, 13);
            this.varKias.TabIndex = 4;
            // 
            // varAltitude
            // 
            this.varAltitude.Location = new System.Drawing.Point(121, 14);
            this.varAltitude.Name = "varAltitude";
            this.varAltitude.Size = new System.Drawing.Size(45, 13);
            this.varAltitude.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(6, 42);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(109, 15);
            this.label1.TabIndex = 2;
            this.label1.Text = "Kohlsman Setting:";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(6, 27);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(109, 15);
            this.label3.TabIndex = 1;
            this.label3.Text = "Indicated Airspeed:";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(6, 14);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(109, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "Altitude:";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(12, 367);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(320, 240);
            this.pictureBox1.TabIndex = 4;
            this.pictureBox1.TabStop = false;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(12, 613);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(114, 23);
            this.button2.TabIndex = 5;
            this.button2.Text = "Draw Thin Lines";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(218, 613);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(114, 23);
            this.button3.TabIndex = 6;
            this.button3.Text = "Draw Thick Lines";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(12, 12);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(130, 23);
            this.button4.TabIndex = 7;
            this.button4.Text = "Connect USB Interface";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Location = new System.Drawing.Point(148, 12);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(94, 21);
            this.comboBox1.TabIndex = 8;
            // 
            // button5
            // 
            this.button5.Location = new System.Drawing.Point(257, 12);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(75, 23);
            this.button5.TabIndex = 9;
            this.button5.Text = "Refresh";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // button6
            // 
            this.button6.Location = new System.Drawing.Point(589, 340);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(75, 23);
            this.button6.TabIndex = 10;
            this.button6.Text = "Send";
            this.button6.UseVisualStyleBackColor = true;
            this.button6.Click += new System.EventHandler(this.button6_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(346, 342);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(237, 20);
            this.textBox1.TabIndex = 11;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(346, 12);
            this.textBox2.Multiline = true;
            this.textBox2.Name = "textBox2";
            this.textBox2.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox2.Size = new System.Drawing.Size(393, 322);
            this.textBox2.TabIndex = 12;
            // 
            // button7
            // 
            this.button7.Location = new System.Drawing.Point(664, 340);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(75, 23);
            this.button7.TabIndex = 13;
            this.button7.Text = "Clear";
            this.button7.UseVisualStyleBackColor = true;
            this.button7.Click += new System.EventHandler(this.button7_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label18);
            this.groupBox2.Controls.Add(this.label16);
            this.groupBox2.Controls.Add(this.label15);
            this.groupBox2.Controls.Add(this.label14);
            this.groupBox2.Controls.Add(this.label13);
            this.groupBox2.Controls.Add(this.label12);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.testCmdSendSweep);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.testCmdSweepPeriod);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.testCmdSweepStep);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.testCmdSweepTo);
            this.groupBox2.Controls.Add(this.testCmdSweepFrom);
            this.groupBox2.Controls.Add(this.testCmdSendOnce);
            this.groupBox2.Controls.Add(this.testCmdValue);
            this.groupBox2.Controls.Add(this.testCmdName);
            this.groupBox2.Location = new System.Drawing.Point(12, 183);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(320, 180);
            this.groupBox2.TabIndex = 14;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Test Command";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(8, 160);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(77, 13);
            this.label18.TabIndex = 36;
            this.label18.Text = "PARAM BANK";
            this.label18.DoubleClick += new System.EventHandler(this.label18_DoubleClick);
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(8, 147);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(80, 13);
            this.label16.TabIndex = 35;
            this.label16.Text = "PARAM PITCH";
            this.label16.DoubleClick += new System.EventHandler(this.label16_DoubleClick);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(175, 119);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(97, 13);
            this.label15.TabIndex = 34;
            this.label15.Text = "9800, 10200, 1, 40";
            this.label15.DoubleClick += new System.EventHandler(this.label15_DoubleClick);
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(175, 106);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(115, 13);
            this.label14.TabIndex = 33;
            this.label14.Text = "15000, 13000, 10, 100";
            this.label14.DoubleClick += new System.EventHandler(this.label14_DoubleClick);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(175, 93);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(109, 13);
            this.label13.TabIndex = 32;
            this.label13.Text = "10000, 12000, 5, 100";
            this.label13.DoubleClick += new System.EventHandler(this.label13_DoubleClick);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(175, 80);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(115, 13);
            this.label12.TabIndex = 31;
            this.label12.Text = "7000, 9000, 125, 1000";
            this.label12.DoubleClick += new System.EventHandler(this.label12_DoubleClick);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(175, 141);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(101, 13);
            this.label11.TabIndex = 30;
            this.label11.Text = "PARAM ALTITUDE";
            this.label11.DoubleClick += new System.EventHandler(this.label11_DoubleClick);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(8, 128);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(72, 13);
            this.label10.TabIndex = 29;
            this.label10.Text = "PARAM KIAS";
            this.label10.DoubleClick += new System.EventHandler(this.label10_DoubleClick);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 93);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(91, 13);
            this.label9.TabIndex = 28;
            this.label9.Text = "300, 900, 10, 250";
            this.label9.DoubleClick += new System.EventHandler(this.label9_DoubleClick);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 80);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(97, 13);
            this.label8.TabIndex = 27;
            this.label8.Text = "0, 1000, 100, 1000";
            this.label8.DoubleClick += new System.EventHandler(this.label8_DoubleClick);
            // 
            // testCmdSendSweep
            // 
            this.testCmdSendSweep.Location = new System.Drawing.Point(228, 58);
            this.testCmdSendSweep.Name = "testCmdSendSweep";
            this.testCmdSendSweep.Size = new System.Drawing.Size(48, 19);
            this.testCmdSendSweep.TabIndex = 26;
            this.testCmdSendSweep.Text = "Sweep";
            this.testCmdSendSweep.UseVisualStyleBackColor = true;
            this.testCmdSendSweep.Click += new System.EventHandler(this.testCmdSendSweep_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(169, 42);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(40, 13);
            this.label7.TabIndex = 25;
            this.label7.Text = "Period:";
            // 
            // testCmdSweepPeriod
            // 
            this.testCmdSweepPeriod.Location = new System.Drawing.Point(172, 58);
            this.testCmdSweepPeriod.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.testCmdSweepPeriod.Name = "testCmdSweepPeriod";
            this.testCmdSweepPeriod.Size = new System.Drawing.Size(50, 20);
            this.testCmdSweepPeriod.TabIndex = 24;
            this.testCmdSweepPeriod.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(113, 42);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(32, 13);
            this.label6.TabIndex = 23;
            this.label6.Text = "Step:";
            // 
            // testCmdSweepStep
            // 
            this.testCmdSweepStep.Location = new System.Drawing.Point(116, 58);
            this.testCmdSweepStep.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.testCmdSweepStep.Name = "testCmdSweepStep";
            this.testCmdSweepStep.Size = new System.Drawing.Size(50, 20);
            this.testCmdSweepStep.TabIndex = 22;
            this.testCmdSweepStep.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.testCmdSweepStep.ValueChanged += new System.EventHandler(this.testCmdSweepStep_ValueChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(57, 42);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(23, 13);
            this.label5.TabIndex = 21;
            this.label5.Text = "To:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(3, 42);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(36, 13);
            this.label4.TabIndex = 20;
            this.label4.Text = "From: ";
            // 
            // testCmdSweepTo
            // 
            this.testCmdSweepTo.Location = new System.Drawing.Point(60, 58);
            this.testCmdSweepTo.Maximum = new decimal(new int[] {
            99999,
            0,
            0,
            0});
            this.testCmdSweepTo.Name = "testCmdSweepTo";
            this.testCmdSweepTo.Size = new System.Drawing.Size(50, 20);
            this.testCmdSweepTo.TabIndex = 19;
            this.testCmdSweepTo.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // testCmdSweepFrom
            // 
            this.testCmdSweepFrom.Location = new System.Drawing.Point(6, 58);
            this.testCmdSweepFrom.Maximum = new decimal(new int[] {
            99999,
            0,
            0,
            0});
            this.testCmdSweepFrom.Name = "testCmdSweepFrom";
            this.testCmdSweepFrom.Size = new System.Drawing.Size(50, 20);
            this.testCmdSweepFrom.TabIndex = 18;
            this.testCmdSweepFrom.ValueChanged += new System.EventHandler(this.testCmdSweepFrom_ValueChanged);
            // 
            // testCmdSendOnce
            // 
            this.testCmdSendOnce.Location = new System.Drawing.Point(228, 19);
            this.testCmdSendOnce.Name = "testCmdSendOnce";
            this.testCmdSendOnce.Size = new System.Drawing.Size(48, 19);
            this.testCmdSendOnce.TabIndex = 17;
            this.testCmdSendOnce.Text = "Send";
            this.testCmdSendOnce.UseVisualStyleBackColor = true;
            this.testCmdSendOnce.Click += new System.EventHandler(this.testCmdSendOnce_Click);
            // 
            // testCmdValue
            // 
            this.testCmdValue.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.testCmdValue.Location = new System.Drawing.Point(172, 19);
            this.testCmdValue.Maximum = new decimal(new int[] {
            99999,
            0,
            0,
            0});
            this.testCmdValue.Minimum = new decimal(new int[] {
            99999,
            0,
            0,
            -2147483648});
            this.testCmdValue.Name = "testCmdValue";
            this.testCmdValue.Size = new System.Drawing.Size(50, 20);
            this.testCmdValue.TabIndex = 16;
            this.testCmdValue.ValueChanged += new System.EventHandler(this.testCmdValue_ValueChanged);
            // 
            // testCmdName
            // 
            this.testCmdName.Location = new System.Drawing.Point(6, 19);
            this.testCmdName.Name = "testCmdName";
            this.testCmdName.Size = new System.Drawing.Size(160, 20);
            this.testCmdName.TabIndex = 15;
            this.testCmdName.Text = "PARAM KIAS";
            // 
            // testCmdSweepTimer
            // 
            this.testCmdSweepTimer.Tick += new System.EventHandler(this.testCmdSweepTimer_Tick);
            // 
            // varDataRate
            // 
            this.varDataRate.Location = new System.Drawing.Point(18, 67);
            this.varDataRate.Name = "varDataRate";
            this.varDataRate.Size = new System.Drawing.Size(36, 13);
            this.varDataRate.TabIndex = 6;
            // 
            // varErrors
            // 
            this.varErrors.Location = new System.Drawing.Point(60, 67);
            this.varErrors.Name = "varErrors";
            this.varErrors.Size = new System.Drawing.Size(36, 13);
            this.varErrors.TabIndex = 15;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.radioButton3);
            this.groupBox3.Controls.Add(this.radioButton1);
            this.groupBox3.Controls.Add(this.radioButton2);
            this.groupBox3.Location = new System.Drawing.Point(214, 83);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(117, 93);
            this.groupBox3.TabIndex = 16;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Flight Instrument";
            // 
            // radioButton3
            // 
            this.radioButton3.AutoSize = true;
            this.radioButton3.Location = new System.Drawing.Point(6, 69);
            this.radioButton3.Name = "radioButton3";
            this.radioButton3.Size = new System.Drawing.Size(82, 17);
            this.radioButton3.TabIndex = 2;
            this.radioButton3.Text = "Radio panel";
            this.radioButton3.UseVisualStyleBackColor = true;
            this.radioButton3.CheckedChanged += new System.EventHandler(this.radioButton3_CheckedChanged);
            // 
            // radioButton1
            // 
            this.radioButton1.AutoSize = true;
            this.radioButton1.Location = new System.Drawing.Point(6, 23);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.Size = new System.Drawing.Size(100, 17);
            this.radioButton1.TabIndex = 0;
            this.radioButton1.Text = "Artificial Horizon";
            this.radioButton1.UseVisualStyleBackColor = true;
            this.radioButton1.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
            // 
            // radioButton2
            // 
            this.radioButton2.AutoSize = true;
            this.radioButton2.Checked = true;
            this.radioButton2.Location = new System.Drawing.Point(6, 46);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(94, 17);
            this.radioButton2.TabIndex = 1;
            this.radioButton2.TabStop = true;
            this.radioButton2.Text = "Altitude/speed";
            this.radioButton2.UseVisualStyleBackColor = true;
            this.radioButton2.CheckedChanged += new System.EventHandler(this.radioButton2_CheckedChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(748, 365);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.varErrors);
            this.Controls.Add(this.varDataRate);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.button7);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.button6);
            this.Controls.Add(this.button5);
            this.Controls.Add(this.comboBox1);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.simStatus);
            this.Controls.Add(this.button1);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdSweepPeriod)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdSweepStep)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdSweepTo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdSweepFrom)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.testCmdValue)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label simStatus;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button button7;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox testCmdName;
        private System.Windows.Forms.Button testCmdSendOnce;
        private System.Windows.Forms.NumericUpDown testCmdValue;
        private System.Windows.Forms.Timer testCmdSweepTimer;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button testCmdSendSweep;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown testCmdSweepPeriod;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.NumericUpDown testCmdSweepStep;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown testCmdSweepTo;
        private System.Windows.Forms.NumericUpDown testCmdSweepFrom;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label varBaro;
        private System.Windows.Forms.Label varKias;
        private System.Windows.Forms.Label varAltitude;
        private System.Windows.Forms.Label varDataRate;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label varErrors;
        private System.Windows.Forms.Label varBank;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label varPitch;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RadioButton radioButton3;
        private System.Windows.Forms.RadioButton radioButton1;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label16;
    }
}

