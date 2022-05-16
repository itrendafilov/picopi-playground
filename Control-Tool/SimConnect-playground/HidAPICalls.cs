using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace SimConnect_playground
{
    public class HidAPICalls
    {
        #region WinAPI

        [DllImport("user32.dll")]
        static extern bool ShowWindowAsync(IntPtr hWnd, int nCmdShow);

        [DllImport("setupapi.dll", SetLastError = true)]
        static extern IntPtr SetupDiGetClassDevs(ref Guid ClassGuid, IntPtr Enumerator, IntPtr hwndParent, int Flags);

        [DllImport("setupapi.dll", SetLastError = true)]
        static extern bool SetupDiEnumDeviceInterfaces(IntPtr hDevInfo, IntPtr devInfo, ref Guid interfaceClassGuid, int memberIndex, ref SP_DEVICE_INTERFACE_DATA deviceInterfaceData);

        [DllImport(@"setupapi.dll", SetLastError = true)]
        static extern bool SetupDiGetDeviceInterfaceDetail(IntPtr DeviceInfoSet, ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData, ref SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData, int DeviceInterfaceDetailDataSize, ref int RequiredSize, IntPtr DeviceInfoData);

        [DllImport(@"setupapi.dll", SetLastError = true)]
        static extern bool SetupDiGetDeviceInterfaceDetail(IntPtr DeviceInfoSet, ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData, IntPtr DeviceInterfaceDetailData, int DeviceInterfaceDetailDataSize, ref int RequiredSize, IntPtr DeviceInfoData);

        [DllImport(@"kernel32.dll", SetLastError = true)]
        static extern IntPtr CreateFile(string fileName, uint fileAccess, uint fileShare, FileMapProtection securityAttributes, uint creationDisposition, uint flags, IntPtr overlapped);

        [DllImport("kernel32.dll")]
        static extern bool WriteFile(IntPtr hFile, [Out] byte[] lpBuffer, uint nNumberOfBytesToWrite, ref uint lpNumberOfBytesWritten, [In] ref NativeOverlapped lpOverlapped);

        [DllImport("kernel32.dll")]
        static extern bool ReadFile(IntPtr hFile, [Out] byte[] lpBuffer, uint nNumberOfBytesToRead, ref uint lpNumberOfBytesRead, [In] ref NativeOverlapped lpOverlapped);

        [DllImport("kernel32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool CancelIo(IntPtr hFile);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool GetOverlappedResult(IntPtr hFile, [In] ref NativeOverlapped lpOverlapped, ref uint lpNumberOfBytesTransferred, bool bWait);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern uint WaitForSingleObject(IntPtr hHandle, uint dwMilliseconds);

        [DllImport("kernel32.dll")]
        static extern IntPtr CreateEvent(IntPtr lpEventAttributes, bool bManualReset, bool bInitialState, string lpName);

        [DllImport("hid.dll")]
        static extern void HidD_GetHidGuid(ref Guid Guid);

        [DllImport("hid.dll", SetLastError = true)]
        static extern bool HidD_GetPreparsedData(IntPtr HidDeviceObject, ref IntPtr PreparsedData);

        [DllImport("hid.dll", SetLastError = true)]
        static extern bool HidD_GetAttributes(IntPtr DeviceObject, ref HIDD_ATTRIBUTES Attributes);

        [DllImport("hid.dll", SetLastError = true)]
        static extern bool HidD_GetSerialNumberString(IntPtr DeviceObject, byte[] Buffer, uint BufferLength);

        [DllImport("hid.dll", SetLastError = true)]
        static extern uint HidP_GetCaps(IntPtr PreparsedData, ref HIDP_CAPS Capabilities);

        [DllImport("hid.dll", SetLastError = true)]
        static extern int HidP_GetButtonCaps(HIDP_REPORT_TYPE ReportType, [In, Out] HIDP_BUTTON_CAPS[] ButtonCaps, ref ushort ButtonCapsLength, IntPtr PreparsedData);

        [DllImport("hid.dll", SetLastError = true)]
        static extern int HidP_GetValueCaps(HIDP_REPORT_TYPE ReportType, [In, Out] HIDP_VALUE_CAPS[] ValueCaps, ref ushort ValueCapsLength, IntPtr PreparsedData);

        [DllImport("hid.dll", SetLastError = true)]
        static extern int HidP_MaxUsageListLength(HIDP_REPORT_TYPE ReportType, ushort UsagePage, IntPtr PreparsedData);

        [DllImport("hid.dll", SetLastError = true)]
        static extern int HidP_SetUsages(HIDP_REPORT_TYPE ReportType, ushort UsagePage, short LinkCollection, short Usages, ref int UsageLength, IntPtr PreparsedData, IntPtr Report, int ReportLength);

        [DllImport("hid.dll", SetLastError = true)]
        static extern int HidP_SetUsageValue(HIDP_REPORT_TYPE ReportType, ushort UsagePage, short LinkCollection, ushort Usage, ulong UsageValue, IntPtr PreparsedData, IntPtr Report, int ReportLength);

        [DllImport("setupapi.dll", SetLastError = true)]
        static extern bool SetupDiDestroyDeviceInfoList(IntPtr DeviceInfoSet);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll")]
        static extern IntPtr GlobalFree(object hMem);

        [DllImport("hid.dll", SetLastError = true)]
        static extern bool HidD_FreePreparsedData(ref IntPtr PreparsedData);

        [DllImport("kernel32.dll")]
        static extern uint GetLastError();

        #endregion

        #region Global Variables
        int SW_SHOW = 5;
        bool cancel = true;
        bool HID_quit = false;
        int nbrDevices = 0;
        int iHIDD;
        bool isConnected = false;

        ushort USAGE_PAGE;
        ushort USAGE;
        HID_DEVICE[] hidDevices;
        #endregion

        #region DLL Var

        IntPtr hardwareDeviceInfo;



        const int DIGCF_DEFAULT = 0x00000001;
        const int DIGCF_PRESENT = 0x00000002;
        const int DIGCF_ALLCLASSES = 0x00000004;
        const int DIGCF_PROFILE = 0x00000008;
        const int DIGCF_DEVICEINTERFACE = 0x00000010;

        const uint GENERIC_READ = 0x80000000;
        const uint GENERIC_WRITE = 0x40000000;
        const uint GENERIC_EXECUTE = 0x20000000;
        const uint GENERIC_ALL = 0x10000000;

        const uint FILE_SHARE_READ = 0x00000001;
        const uint FILE_SHARE_WRITE = 0x00000002;
        const uint FILE_SHARE_DELETE = 0x00000004;

        const uint FILE_FLAG_DUPLEX= 0x00000003;
        const uint FILE_FLAG_OVERLAPPED = 0x40000000;

        const uint ERROR_IO_PENDING = 0x03E5;

        const uint INFINITE = 0xFFFFFFFF;
        const uint WAIT_ABANDONED = 0x00000080;
        const uint WAIT_OBJECT_0 = 0x00000000;
        const uint WAIT_TIMEOUT = 0x00000102;

        const uint CREATE_NEW = 1;
        const uint CREATE_ALWAYS = 2;
        const uint OPEN_EXISTING = 3;
        const uint OPEN_ALWAYS = 4;
        const uint TRUNCATE_EXISTING = 5;

        const int HIDP_STATUS_SUCCESS = 1114112;
        const int DEVICE_PATH = 260;
        const int INVALID_HANDLE_VALUE = -1;

        enum FileMapProtection : uint
        {
            PageReadonly = 0x02,
            PageReadWrite = 0x04,
            PageWriteCopy = 0x08,
            PageExecuteRead = 0x20,
            PageExecuteReadWrite = 0x40,
            SectionCommit = 0x8000000,
            SectionImage = 0x1000000,
            SectionNoCache = 0x10000000,
            SectionReserve = 0x4000000,
        }

        enum HIDP_REPORT_TYPE : ushort
        {
            HidP_Input = 0x00,
            HidP_Output = 0x01,
            HidP_Feature = 0x02,
        }

        [StructLayout(LayoutKind.Sequential)]
        struct LIST_ENTRY
        {
            public IntPtr Flink;
            public IntPtr Blink;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct DEVICE_LIST_NODE
        {
            public LIST_ENTRY Hdr;
            public IntPtr NotificationHandle;
            public HID_DEVICE HidDeviceInfo;
            public bool DeviceOpened;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct SP_DEVICE_INTERFACE_DATA
        {
            public Int32 cbSize;
            public Guid interfaceClassGuid;
            public Int32 flags;
            private UIntPtr reserved;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        struct SP_DEVICE_INTERFACE_DETAIL_DATA
        {
            public int cbSize;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = DEVICE_PATH)]
            public string DevicePath;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct SP_DEVINFO_DATA
        {
            public int cbSize;
            public Guid classGuid;
            public UInt32 devInst;
            public IntPtr reserved;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct HIDP_CAPS
        {
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 Usage;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 UsagePage;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 InputReportByteLength;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 OutputReportByteLength;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 FeatureReportByteLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 17)]
            public UInt16[] Reserved;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberLinkCollectionNodes;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberInputButtonCaps;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberInputValueCaps;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberInputDataIndices;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberOutputButtonCaps;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberOutputValueCaps;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberOutputDataIndices;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberFeatureButtonCaps;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberFeatureValueCaps;
            [MarshalAs(UnmanagedType.U2)]
            public UInt16 NumberFeatureDataIndices;
        };

        [StructLayout(LayoutKind.Sequential)]
        struct HIDD_ATTRIBUTES
        {
            public Int32 Size;
            public UInt16 VendorID;
            public UInt16 ProductID;
            public Int16 VersionNumber;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ButtonData
        {
            public Int32 UsageMin;
            public Int32 UsageMax;
            public Int32 MaxUsageLength;
            public Int16 Usages;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ValueData
        {
            public ushort Usage;
            public ushort Reserved;

            public ulong Value;
            public long ScaledValue;
        }

        [StructLayout(LayoutKind.Explicit)]
        struct HID_DATA
        {
            [FieldOffset(0)]
            public bool IsButtonData;
            [FieldOffset(1)]
            public byte Reserved;
            [FieldOffset(2)]
            public ushort UsagePage;
            [FieldOffset(4)]
            public Int32 Status;
            [FieldOffset(8)]
            public Int32 ReportID;
            [FieldOffset(16)]
            public bool IsDataSet;

            [FieldOffset(17)]
            public ButtonData ButtonData;
            [FieldOffset(17)]
            public ValueData ValueData;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct HIDP_Range
        {
            public ushort UsageMin, UsageMax;
            public ushort StringMin, StringMax;
            public ushort DesignatorMin, DesignatorMax;
            public ushort DataIndexMin, DataIndexMax;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct HIDP_NotRange
        {
            public ushort Usage, Reserved1;
            public ushort StringIndex, Reserved2;
            public ushort DesignatorIndex, Reserved3;
            public ushort DataIndex, Reserved4;
        }

        [StructLayout(LayoutKind.Explicit)]
        struct HIDP_BUTTON_CAPS
        {
            [FieldOffset(0)]
            public ushort UsagePage;
            [FieldOffset(2)]
            public byte ReportID;
            [FieldOffset(3), MarshalAs(UnmanagedType.U1)]
            public bool IsAlias;
            [FieldOffset(4)]
            public short BitField;
            [FieldOffset(6)]
            public short LinkCollection;
            [FieldOffset(8)]
            public short LinkUsage;
            [FieldOffset(10)]
            public short LinkUsagePage;
            [FieldOffset(12), MarshalAs(UnmanagedType.U1)]
            public bool IsRange;
            [FieldOffset(13), MarshalAs(UnmanagedType.U1)]
            public bool IsStringRange;
            [FieldOffset(14), MarshalAs(UnmanagedType.U1)]
            public bool IsDesignatorRange;
            [FieldOffset(15), MarshalAs(UnmanagedType.U1)]
            public bool IsAbsolute;
            [FieldOffset(16), MarshalAs(UnmanagedType.ByValArray, SizeConst = 10)]
            public int[] Reserved;
            [FieldOffset(56)]
            public HIDP_Range Range;
            [FieldOffset(56)]
            public HIDP_NotRange NotRange;
        }

        [StructLayout(LayoutKind.Explicit)]
        struct HIDP_VALUE_CAPS
        {
            [FieldOffset(0)]
            public ushort UsagePage;
            [FieldOffset(2)]
            public byte ReportID;
            [FieldOffset(3), MarshalAs(UnmanagedType.U1)]
            public bool IsAlias;
            [FieldOffset(4)]
            public ushort BitField;
            [FieldOffset(6)]
            public ushort LinkCollection;
            [FieldOffset(8)]
            public ushort LinkUsage;
            [FieldOffset(10)]
            public ushort LinkUsagePage;
            [FieldOffset(12), MarshalAs(UnmanagedType.U1)]
            public bool IsRange;
            [FieldOffset(13), MarshalAs(UnmanagedType.U1)]
            public bool IsStringRange;
            [FieldOffset(14), MarshalAs(UnmanagedType.U1)]
            public bool IsDesignatorRange;
            [FieldOffset(15), MarshalAs(UnmanagedType.U1)]
            public bool IsAbsolute;
            [FieldOffset(16), MarshalAs(UnmanagedType.U1)]
            public bool HasNull;
            [FieldOffset(17)]
            public byte Reserved;
            [FieldOffset(18)]
            public short BitSize;
            [FieldOffset(20)]
            public short ReportCount;
            [FieldOffset(22)]
            public ushort Reserved2a;
            [FieldOffset(24)]
            public ushort Reserved2b;
            [FieldOffset(26)]
            public ushort Reserved2c;
            [FieldOffset(28)]
            public ushort Reserved2d;
            [FieldOffset(30)]
            public ushort Reserved2e;
            [FieldOffset(32)]
            public int UnitsExp;
            [FieldOffset(36)]
            public int Units;
            [FieldOffset(40)]
            public int LogicalMin;
            [FieldOffset(44)]
            public int LogicalMax;
            [FieldOffset(48)]
            public int PhysicalMin;
            [FieldOffset(52)]
            public int PhysicalMax;
            [FieldOffset(56)]
            public HIDP_Range Range;
            [FieldOffset(56)]
            public HIDP_NotRange NotRange;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        struct HID_DEVICE
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = DEVICE_PATH)]
            public string DevicePath;
            public IntPtr HidDevice;
            public bool OpenedForRead;
            public bool OpenedForWrite;
            public bool OpenedOverlapped;
            public bool OpenedExclusive;

            public IntPtr Ppd;
            public HIDP_CAPS Caps;
            public HIDD_ATTRIBUTES Attributes;

            public IntPtr[] InputReportBuffer;
            public HID_DATA[] InputData;
            public Int32 InputDataLength;
            public HIDP_BUTTON_CAPS[] InputButtonCaps;
            public HIDP_VALUE_CAPS[] InputValueCaps;

            public IntPtr[] OutputReportBuffer;
            public HID_DATA[] OutputData;
            public Int32 OutputDataLength;
            public HIDP_BUTTON_CAPS[] OutputButtonCaps;
            public HIDP_VALUE_CAPS[] OutputValueCaps;

            public IntPtr[] FeatureReportBuffer;
            public HID_DATA[] FeatureData;
            public Int32 FeatureDataLength;
            public HIDP_BUTTON_CAPS[] FeatureButtonCaps;
            public HIDP_VALUE_CAPS[] FeatureValueCaps;
        }

        #endregion

        #region Constants
        /*
                private const int DigcfDeviceinterface = 16;
                private const int DigcfPresent = 2;
                private const uint FileShareRead = 1;
                private const uint FileShareWrite = 2;
                private const uint OpenExisting = 3;
                private const int FileAttributeNormal = 128;
                private const int FileFlagOverlapped = 1073741824;
                private const int HIDP_STATUS_SUCCESS = 0x110000;
        */
        #endregion

        #region Constructors
        #endregion

        #region Private Static Fields
        private static Guid? _HidGuid;
        #endregion

        #region Private Static Methods
        public static void HandleError(bool isSuccess, string message)
        {
            if (isSuccess) return;
            var errorCode = Marshal.GetLastWin32Error();

            //TODO: Loggin
            if (errorCode == 0) return;

            throw new Exception($"{message}. Error code: {errorCode}");
        }

        /*        private static string GetHidString(SafeFileHandle safeFileHandle, GetString getString)
                {
                    var pointerToBuffer = Marshal.AllocHGlobal(126);
                    var isSuccess = getString(safeFileHandle, pointerToBuffer, 126);
                    Marshal.FreeHGlobal(pointerToBuffer);
                    HandleError(isSuccess, "Could not get Hid string");
                    return Marshal.PtrToStringUni(pointerToBuffer);
                }*/
        #endregion

        #region Public Static Methods
        /*        public static HidAttributes GetHidAttributes(SafeFileHandle safeFileHandle)
                {
                    var isSuccess = HidD_GetAttributes(safeFileHandle, out var hidAttributes);
                    HandleError(isSuccess, "Could not get Hid Attributes");
                    return hidAttributes;
                }

                public static HidCollectionCapabilities GetHidCapabilities(SafeFileHandle readSafeFileHandle)
                {
                    var isSuccess = HidD_GetPreparsedData(readSafeFileHandle, out var pointerToPreParsedData);
                    HandleError(isSuccess, "Could not get pre parsed data");

                    var result = HidP_GetCaps(pointerToPreParsedData, out var hidCollectionCapabilities);
                    if (result != HIDP_STATUS_SUCCESS)
                    {
                        throw new Exception($"Could not get Hid capabilities. Return code: {result}");
                    }

                    isSuccess = HidD_FreePreparsedData(ref pointerToPreParsedData);
                    HandleError(isSuccess, "Could not release handle for getting Hid capabilities");

                    return hidCollectionCapabilities;
                }

                public static string GetManufacturer(SafeFileHandle safeFileHandle)
                {
                    return GetHidString(safeFileHandle, HidD_GetManufacturerString);
                }

                public static string GetProduct(SafeFileHandle safeFileHandle)
                {
                    return GetHidString(safeFileHandle, HidD_GetProductString);
                }

                public static string GetSerialNumber(SafeFileHandle safeFileHandle)
                {
                    return GetHidString(safeFileHandle, HidD_GetSerialNumberString);
                }

                public static Guid GetHidGuid()
                {
                    if (_HidGuid.HasValue)
                    {
                        return _HidGuid.Value;
                    }

                    Guid hidGuid = new Guid();

                    HidD_GetHidGuid(ref hidGuid);

                    _HidGuid = hidGuid;

                    return hidGuid;
                }

                public static Stream OpenRead(SafeFileHandle readSafeFileHandle, ushort readBufferSize)
                {
                    return new FileStream(readSafeFileHandle, FileAccess.Read, readBufferSize, false);
                }

                public static Stream OpenWrite(SafeFileHandle writeSafeFileHandle, ushort writeBufferSize)
                {
                    return new FileStream(writeSafeFileHandle, FileAccess.ReadWrite, writeBufferSize, false);
                }

                public static SafeFileHandle CreateWriteConnection(string deviceId)
                {
                    return CreateConnection(deviceId, FileAccessRights.GenericRead | FileAccessRights.GenericWrite, FileShareRead | FileShareWrite, OpenExisting);
                }

                public static SafeFileHandle CreateReadConnection(string deviceId, FileAccessRights desiredAccess)
                {
                    return CreateConnection(deviceId, desiredAccess, FileShareRead | FileShareWrite, OpenExisting);
                }*/
        #endregion

        #region Public Methods
        public int getNumberOfInterfaces(ushort vid, ushort pid)
        {
            RefreshDeviceList();

            int count = 0;
            var i = 0;
            while (i < nbrDevices)
            {
                HID_DEVICE pDevice = hidDevices[i];

                if (pDevice.Attributes.VendorID == vid && vid != 0)
                {
                    if (pDevice.Attributes.ProductID == pid)
                    {
                        count++;
                    }
                    /* // What are those?
                                        if (pDevice.Caps.UsagePage == USAGE_PAGE)
                                        {
                                            count++;
                                        }

                                        if (pDevice.Caps.Usage == USAGE)
                                        {
                                            count++;
                                        }
                    */
                }
                i++;
            }
            return count;
        }

        public string[] getSerialsForVidPid(ushort vid, ushort pid)
        {
            RefreshDeviceList();

            const uint bufferLen = 128;
            byte[] buffer = new byte[bufferLen];
            List<String> serials = new List<String>();
            var i = 0;
            while (i < nbrDevices)
            {
                HID_DEVICE pDevice = hidDevices[i];

                if (pDevice.Attributes.VendorID == vid && vid != 0)
                {
                    if (pDevice.Attributes.ProductID == pid)
                    {
                        if (!HidD_GetSerialNumberString(pDevice.HidDevice, buffer, bufferLen))
                        {
                            throw new Exception("Unable to read SerialNumber");
                        }

                        serials.Add(Encoding.Unicode.GetString(buffer, 0, buffer.Length));
                    }
                }
                i++;
            }
            return serials.ToArray();
        }

        public int connectDevice(int vid, int pid, String serial, int infNumber)
        {
            RefreshDeviceList();

            const uint bufferLen = 128;
            byte[] buffer = new byte[bufferLen];

            var i = 0;
            while (i < nbrDevices)
            {
                HID_DEVICE pDevice = hidDevices[i];

                if (pDevice.Attributes.VendorID == vid && vid != 0)
                {
                    if (pDevice.Attributes.ProductID == pid)
                    {
                        if (!HidD_GetSerialNumberString(pDevice.HidDevice, buffer, bufferLen))
                        {
                            throw new Exception("Unable to read SerialNumber");
                        }

                        if (Encoding.Unicode.GetString(buffer, 0, buffer.Length).Equals(serial))
                        {
                            return i;
                        }
                    }
                }
                i++;
            }

            return -1;
        }
        public byte[] receiveData(int handle)
        {
            HID_DEVICE pDevice = hidDevices[handle];


            byte[] Report = new byte[pDevice.Caps.InputReportByteLength];
            uint tmp = 0;

            Report[0] = 0x3F;

            // Inspired by https://github.com/signal11/hidapi/blob/master/windows/hid.c
            // ToDo: move that to external and persisted object in connectDevice()
            NativeOverlapped ol = new NativeOverlapped();
            IntPtr ev = CreateEvent(IntPtr.Zero, false, false/*inital state f=nonsignaled*/, null);
            ol.EventHandle = ev;
            //ResetEvent(ev);

            if (!ReadFile(pDevice.HidDevice, Report, pDevice.Caps.InputReportByteLength, ref tmp, ref ol))
            {
                if (GetLastError() != ERROR_IO_PENDING)
                {
                    /* ReadFile() has failed.
                       Clean up and return error. */
                    CancelIo(pDevice.HidDevice);
                    // ToDo: Error here?
                    return Report;
                }

            }

            switch (WaitForSingleObject(ev, INFINITE))
            {
                case WAIT_ABANDONED:
                case WAIT_TIMEOUT:
                    return Report;
            }

            if (!GetOverlappedResult(pDevice.HidDevice, ref ol, ref tmp, true/*wait*/))
            {
                // ToDo: Report errors?
            }

            return Report;
        }

        public uint sendData(int handle, byte[] data, uint size)
        {
            HID_DEVICE pDevice = hidDevices[handle];

            NativeOverlapped lpOverlapped = new NativeOverlapped();

            byte[] Report = new byte[pDevice.Caps.OutputReportByteLength];
            uint tmp = 0;
            for (var i = 0; i < size; i++)
            {
                Report[i] = data[i];
            }

            if (!WriteFile(pDevice.HidDevice, Report, pDevice.Caps.OutputReportByteLength, ref tmp, ref lpOverlapped)
                 && Marshal.GetLastWin32Error() == ERROR_IO_PENDING)
            {
                if (!GetOverlappedResult(pDevice.HidDevice, ref lpOverlapped, ref tmp, true))
                {
                    return 0;
                }
            }
            return tmp;
        }
        #endregion

        #region Private Methods
        /*        private static SafeFileHandle CreateConnection(string deviceId, FileAccessRights desiredAccess, uint shareMode, uint creationDisposition)
                {
                    return CreateFile(deviceId, desiredAccess, shareMode, IntPtr.Zero, creationDisposition, 0, IntPtr.Zero);
                }*/

        private void RefreshDeviceList()
        {
            int devNum = FindNumberDevices();
            if (nbrDevices != devNum)
            {
                nbrDevices = devNum;
                HID_DEVICE[] pDevice = new HID_DEVICE[devNum];
                FindKnownHidDevices(ref pDevice);
                hidDevices = pDevice;
            }
        }

        private int FindNumberDevices()
        {
            Guid hidGuid = new Guid();
            SP_DEVICE_INTERFACE_DATA deviceInfoData = new SP_DEVICE_INTERFACE_DATA();
            int index = 0;

            HidD_GetHidGuid(ref hidGuid);

            //
            // Open a handle to the plug and play dev node.
            //
            SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
            hardwareDeviceInfo = SetupDiGetClassDevs(ref hidGuid, IntPtr.Zero, IntPtr.Zero, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
            deviceInfoData.cbSize = Marshal.SizeOf(typeof(SP_DEVICE_INTERFACE_DATA));

            index = 0;
            while (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, IntPtr.Zero, ref hidGuid, index, ref deviceInfoData))
            {
                index++;
            }

            return (index);
        }

        private int FindKnownHidDevices(ref HID_DEVICE[] HidDevices)
        {
            int iHIDD;
            int RequiredLength;

            Guid hidGuid = new Guid();
            SP_DEVICE_INTERFACE_DATA deviceInfoData = new SP_DEVICE_INTERFACE_DATA();
            SP_DEVICE_INTERFACE_DETAIL_DATA functionClassDeviceData = new SP_DEVICE_INTERFACE_DETAIL_DATA();

            HidD_GetHidGuid(ref hidGuid);

            //
            // Open a handle to the plug and play dev node.
            //
            SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
            hardwareDeviceInfo = SetupDiGetClassDevs(ref hidGuid, IntPtr.Zero, IntPtr.Zero, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
            deviceInfoData.cbSize = Marshal.SizeOf(typeof(SP_DEVICE_INTERFACE_DATA));

            iHIDD = 0;
            while (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, IntPtr.Zero, ref hidGuid, iHIDD, ref deviceInfoData))
            {
                RequiredLength = 0;

                //
                // allocate a function class device data structure to receive the
                // goods about this particular device.
                //
                SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, ref deviceInfoData, IntPtr.Zero, 0, ref RequiredLength, IntPtr.Zero);

                if (IntPtr.Size == 8)
                    functionClassDeviceData.cbSize = 8;
                else if (IntPtr.Size == 4)
                    functionClassDeviceData.cbSize = 5;

                //
                // Retrieve the information from Plug and Play.
                //
                SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, ref deviceInfoData, ref functionClassDeviceData, RequiredLength, ref RequiredLength, IntPtr.Zero);

                //
                // Open device with just generic query abilities to begin with
                //
                OpenHidDevice(functionClassDeviceData.DevicePath, ref HidDevices, iHIDD);

                iHIDD++;
            }

            return iHIDD;
        }

        void OpenHidDevice(string DevicePath, ref HID_DEVICE[] HidDevice, int iHIDD)
        {
            /*++
            RoutineDescription:
            Given the HardwareDeviceInfo, representing a handle to the plug and
            play information, and deviceInfoData, representing a specific hid device,
            open that device and fill in all the relivant information in the given
            HID_DEVICE structure.
            --*/

            HidDevice[iHIDD].DevicePath = DevicePath;

            //
            //  The hid.dll api's do not pass the overlapped structure into deviceiocontrol
            //  so to use them we must have a non overlapped device.  If the request is for
            //  an overlapped device we will close the device below and get a handle to an
            //  overlapped device
            //
            CloseHandle(HidDevice[iHIDD].HidDevice);
            HidDevice[iHIDD].HidDevice = CreateFile(HidDevice[iHIDD].DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, IntPtr.Zero);
            HidDevice[iHIDD].Caps = new HIDP_CAPS();
            HidDevice[iHIDD].Attributes = new HIDD_ATTRIBUTES();

            //
            // If the device was not opened as overlapped, then fill in the rest of the
            //  HidDevice structure.  However, if opened as overlapped, this handle cannot
            //  be used in the calls to the HidD_ exported functions since each of these
            //  functions does synchronous I/O.
            //
            HidD_FreePreparsedData(ref HidDevice[iHIDD].Ppd);
            HidDevice[iHIDD].Ppd = IntPtr.Zero;
            HidD_GetPreparsedData(HidDevice[iHIDD].HidDevice, ref HidDevice[iHIDD].Ppd);
            HidD_GetAttributes(HidDevice[iHIDD].HidDevice, ref HidDevice[iHIDD].Attributes);
            HidP_GetCaps(HidDevice[iHIDD].Ppd, ref HidDevice[iHIDD].Caps);

            //MessageBox.Show(GetLastError().ToString());

            //
            // At this point the client has a choice.  It may chose to look at the
            // Usage and Page of the top level collection found in the HIDP_CAPS
            // structure.  In this way --------*it could just use the usages it knows about.
            // If either HidP_GetUsages or HidP_GetUsageValue return an error then
            // that particular usage does not exist in the report.
            // This is most likely the preferred method as the application can only
            // use usages of which it already knows.
            // In this case the app need not even call GetButtonCaps or GetValueCaps.
            //
            // In this example, however, we will call FillDeviceInfo to look for all
            //    of the usages in the device.
            //
            //FillDeviceInfo(ref HidDevice);
        }
        #endregion
    }
}
