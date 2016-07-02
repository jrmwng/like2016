# Branch CPUID

C++ Header File: [cpuid.h](https://github.com/jrmwng/like2016/blob/cpuid/jrmwng/shared/cpuid.h)

[cpuid.h] contains convenient classes (e.g. cpuid_forest_t, cpuid_info_t<0x01>) around CPUID instruction.
For example, to read the ID of the executing logical processor,

    using namespace jrmwng::cpuid;
    
    cpuid_info_t<0x01> cpuid01; // execute CPUID[EAX=1,ECX=0]
    
    unsigned const uID = cpuid01.uInitialAPIC_ID; // CPUID[EAX=1,ECX=0].EBX[31:24]

## Console Application: [cpuid.exe](https://github.com/jrmwng/like2016/blob/cpuid/jrmwng/cpuid)

[cpuid.exe] is a simple console application that output results of executing CPUID instruction.

           0   0       16 756e6547 6c65746e 49656e69 GenuineIntel
           1   0    406e3  3100800 7ffafbff bfebfbff 06_4EH +SSE3 +PCLMULQDQ +DTES64 +MONITOR +DS_CPL +VMX +SMX +EIST +TM2 +SSSE3 -CNXT_ID +SDBG +FMA +CMPXCHG16B +xTPR_UpdateControl +PDCM +PCID -DCA +SSE4.1 +SSE4.2 +x2APIC +MOVBE +POPCNT +TSC_Deadline +AES +XSAVE +OSXSAVE +AVX +F16C +RDRAND +FPU +VME +DE +PSE +TSC +MSR +PAE +MCE +CX8 +APIC +SEP +MTRR +PGE +MCA +CMOV +PAT +PSE36 -PSN +CLFSH +DS +ACPI +MMX +FXSR +SSE +SSE2 +SS +HTT +TM +PBE
           2   0 76036301   f0b5ff        0   c30000
           4   0 1c004121  1c0003f       3f        0 D-Cache L1  2processor(s)  8core(s)  8way(s) * 1partition(s) * 64B *    64set(s)
           4   1 1c004122  1c0003f       3f        0 I-Cache L1  2processor(s)  8core(s)  8way(s) * 1partition(s) * 64B *    64set(s)
           4   2 1c004143   c0003f      3ff        0 U-Cache L2  2processor(s)  8core(s)  4way(s) * 1partition(s) * 64B *  1024set(s)
           4   3 1c03c163  2c0003f      fff        6 U-Cache L3 16processor(s)  8core(s) 12way(s) * 1partition(s) * 64B *  4096set(s)
           5   0       40       40        3 11142120 [64B,64B] +EMX +IBE C0/0 C1/2 C2/1 C3/2 C4/4 C5/1 C6/1 C7/1
           6   0     27f7        2        9        0 +ThermalSensor +TurboBoost +ARAT +PLN +ECMD +PTM +HWP +HDC +SETBH
           7   0        0  29c6fbf        0        0 +FSGSBASE +IA32_TSC_ADJUST +SGX +BMI1 +HLE +AVX2 -FDP_EXCPTN_ONLY +SMEP +BMI2 +FastString +INVPCID +RTM -PQM +SuppressFCS +SuppressFDS +MPX -PQE -AVX512F -AVX512DQ +RDSEED +ADX +SMAP -AVX512IFMA -PCOMMIT +CLFLUSHOPT -CLWB +PT -AVX512PF -AVX512ER -AVX512CD -SHA -AVX512BW -AVX512VL -PREFTEHCHWT1 -AVX512VBMI -PKU -OSPKE
           a   0  7300404        0        0      603 VersionID=4 4*48b +CoreCycleEvent +InstructionRetiredEvent +ReferenceCyclesEvent +LLC_ReferenceEvent +LLC_MissesEvent +BranchInstructionRetiredEvent +BranchMispredictRetiredEvent 3*48b
           b   0        1        2      100        3     SMT x2APIC-ID=3 Topology-ID=1 Logical-Processor=2
           b   1        4        4      201        3    Core x2APIC-ID=3 Topology-ID=0 Logical-Processor=4
           d   0       1f      440      440        0 +x87 +SSE +AVX +BNDREGS +BNDCSR -Opmask -ZMM_Hi256 -Hi16_ZMM -PT -PKRU -LWP 1088B 1088B
           d   1        f      3c0      100        0 +XSAVEOPT +Compaction_Extensions +XGETBV +XSAVES_IA32_XSS 960B
           d   2      100      240        0        0 AVX        256B @  576B
           d   3       40      3c0        0        0 BNGREG      64B @  960B
           d   4       40      400        0        0 BNGCSR      64B @ 1024B
           d   8       80        0        1        0 PT         128B @    0B
          14   0        1        f        7        0
          14   1  2490002   3f3fff        0        0
          15   0        2       d0        0        0 208TSC / 2CLK
          16   0      9c4      bb8       64        0 [2500MHz,3000MHz]@100MHz
    80000000   0 80000008        0        0        0
    80000001   0        0        0      121 2c100800 +LAHF +SAHF -CmpLegacy -SVM -ExtApicSpace -AltMovCr8 +ABM -SSE4A -MisAlignSse +3DNowPrefetch -OSVW -IBS -XOP -SKINIT -WDT -LWP -FMA4 -TBM -TopologyExtensions -PerfCtrExtCore -PerfCtrExtNB -DataBreakpointEx -PerfTsc -MONITORX +SYSCALL +SYSRET +NX -MmxExt -FFXSR +Page1GB +RDTSCP +LM -3DNowExt -3DNow!
    80000002   0 65746e49 2952286c 726f4320 4d542865 Intel(R) Core(TM
    80000003   0 35692029 3033362d 43205530 40205550 ) i5-6300U CPU @
    80000004   0 342e3220 7a484730        0        0  2.40GHz
    80000006   0        0        0  1006040        0 L2:64B*0*8way=256KB
    80000007   0        0        0        0      100 -McaOverflowRecov -SUCCOR -HWA CpuPwrSampleTimeRatio=0 -TS -FID -VID -TTP -TM -100MHzSteps -HwPstate +TscInvariant -CPB -EffFreqR0 -ProcFeedbackInterface -ProcPowerReporting
    80000008   0     3027        0        0        0 PhysAddrSize=39 LinAddrSize=48 GuestPhysAddrSize=0 NC=0 ApicIdCoreIdSize=0 PerfTscSize=0
