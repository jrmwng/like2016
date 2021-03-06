#pragma once

/* Author: jrmwng @ 2015-2016 */

#include <intrin.h>
#include <iostream>
#include <iomanip>

namespace jrmwng
{
	namespace cpuid
	{
		template <unsigned uEAX, unsigned uECX = 0>
		struct cpuid_info_t
		{
			// eax
			unsigned : 32;
			// ebx
			unsigned : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 32;

			bool is_sub_leaf(unsigned uMaxECX) const
			{
				return static_cast<unsigned>(uECX) <= uMaxECX;
			}
		};
		template <unsigned uEAX, unsigned uECX>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<uEAX, uECX> const &)
		{
			return os;
		}

		template <> struct cpuid_info_t<0x00>
		{
			// eax
			unsigned uMaxLeaf : 32;
			// ebx
			char acVendor0[4];
			// ecx
			char acVendor2[4];
			// edx
			char acVendor1[4];

			unsigned max_leaf() const
			{
				return uMaxLeaf;
			}
		};
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x00> const & cpuid)
		{
			return (os << ' ').write(cpuid.acVendor0, 4).write(cpuid.acVendor1, 4).write(cpuid.acVendor2, 4);
		}

		template <> struct cpuid_info_t<0x01>
		{
			// eax
			unsigned uSteppingID : 4; // [bits 3:0]
			unsigned uModelID : 4; // [bits 7:4]
			unsigned uFamilyID : 4; // [bits 11:8]
			enum processor_type : unsigned
			{
				ORIGINAL_OEM_PROCESSOR = 0,
				INTEL_OVERDRIVE_PROCESSOR = 1,
				DUAL_PROCESSOR = 2,
				INTEL_RESERVED = 3,
			} emProcessorType : 2;
			unsigned : 2; // [bits 15:14]
			unsigned uExtendedModelID : 4; // [bits 19:16]
			unsigned uExtendedFamilyID : 8; // [bits 27:20]
			unsigned : 4; // [bits 31:28]

			unsigned family() const
			{
				return uFamilyID + uExtendedFamilyID;
			}
			unsigned model() const
			{
				return (uExtendedModelID << 4) | uModelID;
			}

			// ebx
			unsigned uProcessorBrandIndex : 8; // [bits 7:0]
			unsigned uLineSizeCLFLUSH : 8; // [bits 15:8]
			unsigned uMaxNumOfID : 8; // [bits 23:16]
			unsigned uInitialAPIC_ID : 8; // [bits 31:24]
										  // ecx
			unsigned uSSE3 : 1; // bit 0
			unsigned uPCLMULQDQ : 1; // bit 1
			unsigned uDTES64 : 1; // bit 2
			unsigned uMONITOR : 1; // bit 3
			unsigned uDS_CPL : 1; // bit 4
			unsigned uVMX : 1; // bit 5
			unsigned uSMX : 1; // bit 6: Safer Mode Extensions
			unsigned uEIST : 1; // bit 7: Enhanced Intel SpeedStep technology
			unsigned uTM2 : 1; // bit 8: Thermal Monitor 2
			unsigned uSSSE3 : 1; // bit 9
			unsigned uCNXT_ID : 1; // bit 10: L1 Context ID
			unsigned uSDBG : 1; // bit 11
			unsigned uFMA : 1; // bit 12
			unsigned uCMPXCHG16B : 1; // bit 13
			unsigned u_xTPR_UpdateControl : 1; // bit 14
			unsigned uPDCM : 1; // bit 15: Perfmon and Debug Capability
			unsigned : 1; // bit 16
			unsigned uPCID : 1; // bit 17: Process-context identifiers
			unsigned uDCA : 1; // bit 18
			unsigned uSSE4_1 : 1; // bit 19
			unsigned uSSE4_2 : 1; // bit 20
			unsigned u_x2APIC : 1; // bit 21
			unsigned uMOVBE : 1; // bit 22
			unsigned uPOPCNT : 1; // bit 23
			unsigned uTSC_Deadline : 1; // bit 24
			unsigned uAES : 1; // bit 25
			unsigned uXSAVE : 1; // bit 26
			unsigned uOSXSAVE : 1; // bit 27
			unsigned uAVX : 1; // bit 28
			unsigned uF16C : 1; // bit 29
			unsigned uRDRAND : 1; // bit 30
			unsigned uHypervisorPresent : 1;
			// edx
			unsigned uFPU : 1; // bit 0
			unsigned uVME : 1; // bit 1: Virtual 8086 Mode Enhancements
			unsigned uDE : 1; // bit 2: Debugging Extensions
			unsigned uPSE : 1; // bit 3: Page Size Extension
			unsigned uTSC : 1; // bit 4
			unsigned uMSR : 1; // bit 5
			unsigned uPAE : 1; // bit 6
			unsigned uMCE : 1; // bit 7: Machine Check Exception
			unsigned uCX8 : 1; // bit 8: CMPXCHG8B Instruction
			unsigned uAPIC : 1; // bit 9
			unsigned : 1; // bit 10
			unsigned uSEP : 1; // bit 11: SYSENTER and SYSEXIT Instructions
			unsigned uMTRR : 1; // bit 12
			unsigned uPGE : 1; // bit 13: Page Global Bit
			unsigned uMCA : 1; // bit 14 Machine Check Architecture
			unsigned uCMOV : 1; // bit 15
			unsigned uPAT : 1; // bit 16: Page Attribute Table
			unsigned uPSE36 : 1; // bit 17: 36-Bit Page Size Extension
			unsigned uPSN : 1; // bit 18: Processor Serial Number
			unsigned uCLFSH : 1; // bit 19
			unsigned : 1; // bit 20
			unsigned uDS : 1; // bit 21: Debug Store
			unsigned uACPI : 1; // bit 22: Thermal Monitor and Software Controlled Clock Facilities
			unsigned uMMX : 1; // bit 23
			unsigned uFXSR : 1; // bit 24
			unsigned uSSE : 1; // bit 25
			unsigned uSSE2 : 1; // bit 26
			unsigned uSS : 1; // bit 27: Self Snoop
			unsigned uHTT : 1; // bit 28: Max APIC IDs reserved field is Valid
			unsigned uTM : 1; // bit 29: Thermal Monitor
			unsigned : 1; // bit 30
			unsigned uPBE : 1; // bit 31: Pending Break Enable
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x01> const & cpuid)
		{
			char const cFill = os.fill('0');
			int const nMasks = os.setf(std::ios_base::hex | std::ios_base::uppercase);
			os.unsetf(std::ios_base::dec);
			os
				<< ' ' << std::setw(2) << cpuid.family()
				<< '_' << std::setw(2) << cpuid.model() << 'H';
			os.flags(nMasks);
			os.fill(cFill);
			return os <<
				(cpuid.uSSE3 ? " +SSE3" : " -SSE3") << //unsigned uSSE3 : 1; // bit 0
				(cpuid.uPCLMULQDQ ? " +PCLMULQDQ" : " -PCLMULQDQ") << //unsigned uPCLMULQDQ : 1; // bit 1
				(cpuid.uDTES64 ? " +DTES64" : " -DTES64") << //unsigned uDTES64 : 1; // bit 2
				(cpuid.uMONITOR ? " +MONITOR" : " -MONITOR") << //unsigned uMONITOR : 1; // bit 3
				(cpuid.uDS_CPL ? " +DS_CPL" : " -DS_CPL") << //unsigned uDS_CPL : 1; // bit 4
				(cpuid.uVMX ? " +VMX" : " -VMX") << //unsigned uVMX : 1; // bit 5
				(cpuid.uSMX ? " +SMX" : " -SMX") << //unsigned uSMX : 1; // bit 6: Safer Mode Extensions
				(cpuid.uEIST ? " +EIST" : " -EIST") << //unsigned uEIST : 1; // bit 7: Enhanced Intel SpeedStep technology
				(cpuid.uTM2 ? " +TM2" : " -TM2") << //unsigned uTM2 : 1; // bit 8: Thermal Monitor 2
				(cpuid.uSSSE3 ? " +SSSE3" : " -SSSE3") << //unsigned uSSSE3 : 1; // bit 9
				(cpuid.uCNXT_ID ? " +CNXT_ID" : " -CNXT_ID") << //unsigned uCNXT_ID : 1; // bit 10: L1 Context ID
				(cpuid.uSDBG ? " +SDBG" : " -SDBG") << //unsigned uSDBG : 1; // bit 11
				(cpuid.uFMA ? " +FMA" : " -FMA") << //unsigned uFMA : 1; // bit 12
				(cpuid.uCMPXCHG16B ? " +CMPXCHG16B" : " -CMPXCHG16B") << //unsigned uCMPXCHG16B : 1; // bit 13
				(cpuid.u_xTPR_UpdateControl ? " +xTPR_UpdateControl" : " -xTPR_UpdateControl") << //unsigned u_xTPR_UpdateControl : 1; // bit 14
				(cpuid.uPDCM ? " +PDCM" : " -PDCM") << //unsigned uPDCM : 1; // bit 15: Perfmon and Debug Capability
													   //unsigned : 1; // bit 16
				(cpuid.uPCID ? " +PCID" : " -PCID") << //unsigned uPCID : 1; // bit 17: Process-context identifiers
				(cpuid.uDCA ? " +DCA" : " -DCA") << //unsigned uDCA : 1; // bit 18
				(cpuid.uSSE4_1 ? " +SSE4.1" : " -SSE4.1") << //unsigned uSSE4_1 : 1; // bit 19
				(cpuid.uSSE4_2 ? " +SSE4.2" : " -SSE4.2") << //unsigned uSSE4_2 : 1; // bit 20
				(cpuid.u_x2APIC ? " +x2APIC" : " -x2APIC") << //unsigned u_x2APIC : 1; // bit 21
				(cpuid.uMOVBE ? " +MOVBE" : " -MOVBE") << //unsigned uMOVBE : 1; // bit 22
				(cpuid.uPOPCNT ? " +POPCNT" : " -POPCNT") << //unsigned uPOPCNT : 1; // bit 23
				(cpuid.uTSC_Deadline ? " +TSC_Deadline" : " -TSC_Deadline") << //unsigned uTSC_Deadline : 1; // bit 24
				(cpuid.uAES ? " +AES" : " -AES") << //unsigned uAES : 1; // bit 25
				(cpuid.uXSAVE ? " +XSAVE" : " -XSAVE") << //unsigned uXSAVE : 1; // bit 26
				(cpuid.uOSXSAVE ? " +OSXSAVE" : " -OSXSAVE") << //unsigned uOSXSAVE : 1; // bit 27
				(cpuid.uAVX ? " +AVX" : " -AVX") << //unsigned uAVX : 1; // bit 28
				(cpuid.uF16C ? " +F16C" : " -F16C") << //unsigned uF16C : 1; // bit 29
				(cpuid.uRDRAND ? " +RDRAND" : " -RDRAND") << //unsigned uRDRAND : 1; // bit 30
															 //unsigned : 1;
				(cpuid.uFPU ? " +FPU" : " -FPU") << //unsigned uFPU : 1; // bit 0
				(cpuid.uVME ? " +VME" : " -VME") << //unsigned uVME : 1; // bit 1: Virtual 8086 Mode Enhancements
				(cpuid.uDE ? " +DE" : " -DE") << //unsigned uDE : 1; // bit 2: Debugging Extensions
				(cpuid.uPSE ? " +PSE" : " -PSE") << //unsigned uPSE : 1; // bit 3: Page Size Extension
				(cpuid.uTSC ? " +TSC" : " -TSC") << //unsigned uTSC : 1; // bit 4
				(cpuid.uMSR ? " +MSR" : " -MSR") << //unsigned uMSR : 1; // bit 5
				(cpuid.uPAE ? " +PAE" : " -PAE") << //unsigned uPAE : 1; // bit 6
				(cpuid.uMCE ? " +MCE" : " -MCE") << //unsigned uMCE : 1; // bit 7: Machine Check Exception
				(cpuid.uCX8 ? " +CX8" : " -CX8") << //unsigned uCX8 : 1; // bit 8: CMPXCHG8B Instruction
				(cpuid.uAPIC ? " +APIC" : " -APIC") << //unsigned uAPIC : 1; // bit 9
													   //unsigned : 1; // bit 10
				(cpuid.uSEP ? " +SEP" : " -SEP") << //unsigned uSEP : 1; // bit 11: SYSENTER and SYSEXIT Instructions
				(cpuid.uMTRR ? " +MTRR" : " -MTRR") << //unsigned uMTRR : 1; // bit 12
				(cpuid.uPGE ? " +PGE" : " -PGE") << //unsigned uPGE : 1; // bit 13: Page Global Bit
				(cpuid.uMCA ? " +MCA" : " -MCA") << //unsigned uMCA : 1; // bit 14 Machine Check Architecture
				(cpuid.uCMOV ? " +CMOV" : " -CMOV") << //unsigned uCMOV : 1; // bit 15
				(cpuid.uPAT ? " +PAT" : " -PAT") << //unsigned uPAT : 1; // bit 16: Page Attribute Table
				(cpuid.uPSE36 ? " +PSE36" : " -PSE36") << //unsigned uPSE36 : 1; // bit 17: 36-Bit Page Size Extension
				(cpuid.uPSN ? " +PSN" : " -PSN") << //unsigned uPSN : 1; // bit 18: Processor Serial Number
				(cpuid.uCLFSH ? " +CLFSH" : " -CLFSH") << //unsigned uCLFSH : 1; // bit 19
														  //unsigned : 1; // bit 20
				(cpuid.uDS ? " +DS" : " -DS") << //unsigned uDS : 1; // bit 21: Debug Store
				(cpuid.uACPI ? " +ACPI" : " -ACPI") << //unsigned uACPI : 1; // bit 22: Thermal Monitor and Software Controlled Clock Facilities
				(cpuid.uMMX ? " +MMX" : " -MMX") << //unsigned uMMX : 1; // bit 23
				(cpuid.uFXSR ? " +FXSR" : " -FXSR") << //unsigned uFXSR : 1; // bit 24
				(cpuid.uSSE ? " +SSE" : " -SSE") << //unsigned uSSE : 1; // bit 25
				(cpuid.uSSE2 ? " +SSE2" : " -SSE2") << //unsigned uSSE2 : 1; // bit 26
				(cpuid.uSS ? " +SS" : " -SS") << //unsigned uSS : 1; // bit 27: Self Snoop
				(cpuid.uHTT ? " +HTT" : " -HTT") << //unsigned uHTT : 1; // bit 28: Max APIC IDs reserved field is Valid
				(cpuid.uTM ? " +TM" : " -TM") << //unsigned uTM : 1; // bit 29: Thermal Monitor
												 //unsigned : 1; // bit 30
				(cpuid.uPBE ? " +PBE" : " -PBE");//unsigned uPBE : 1; // bit 31: Pending Break Enable
		}
		template <unsigned uECX> struct cpuid_info_t<0x04, uECX>
		{
			// eax
			enum cache_type : unsigned
			{
				NO_MORE_CACHES = 0,
				DATA_CACHE = 1,
				INSTRUCTION_CACHE = 2,
				UNIFIED_CACHE = 3,
			} emCacheTypeField : 5; // [bits 04:00]
			unsigned uCacheLevel : 3; // [bits 07:05]
			unsigned uSelfInitializingCacheLevel : 1; // bit 8
			unsigned uFullyAssociativeCache : 1; // bit 9
			unsigned : 4; // [bits 13:10]
			unsigned uMaximumNumberOfAddressableIDsForLogicalProcessors : 12; // [bits 25:14]
			unsigned uMaximumNumberOfAddressableIDsForProcessorCores : 6; // [bits 31:26]
																		  // ebx
			unsigned uSystemCoherencyLineSize : 12; // [bits 11:0]
			unsigned uPhysicalLinePartitions : 10; // [bits 21:12]
			unsigned uWaysOfAssociativity : 10; // [bits 31:22]
												// ecx
			unsigned uNumberOfSets : 32;
			// edx
			unsigned uWriteBackInvalidate_Invalidate : 1; // bit 0
			unsigned uCacheInclusiveness : 1; // bit 1
			unsigned uComplexCacheIndexing : 1; // bit 2
			unsigned : 29;

			unsigned max_sub_leaf() const
			{
				return ~0U;
			}
			bool is_sub_leaf(unsigned) const
			{
				return emCacheTypeField != NO_MORE_CACHES;
			}
		};
		template <unsigned uECX> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x04, uECX> const & cpuid)
		{
			switch (cpuid.emCacheTypeField)
			{
			case cpuid.NO_MORE_CACHES:
				return os;
			case cpuid.DATA_CACHE:
				os << " D-Cache";
				break;
			case cpuid.INSTRUCTION_CACHE:
				os << " I-Cache";
				break;
			case cpuid.UNIFIED_CACHE:
				os << " U-Cache";
				break;
			}
			return os
				<< " L" << (cpuid.uCacheLevel)
				<< ' ' << std::setw(2) << (cpuid.uMaximumNumberOfAddressableIDsForLogicalProcessors + 1) << "processor(s)"
				<< ' ' << std::setw(2) << (cpuid.uMaximumNumberOfAddressableIDsForProcessorCores + 1) << "core(s)"
				<< ' ' << std::setw(2) << (cpuid.uWaysOfAssociativity + 1) << "way(s)"
				<< " * " << std::setw(1) << (cpuid.uPhysicalLinePartitions + 1) << "partition(s)"
				<< " * " << std::setw(2) << (cpuid.uSystemCoherencyLineSize + 1) << 'B'
				<< " * " << std::setw(5) << (cpuid.uNumberOfSets + 1) << "set(s)";
		}
		template <> struct cpuid_info_t<0x05>
		{
			// eax
			unsigned uSmallestLineSizeMONITOR : 16; // [bits 15:0]
			unsigned : 16;
			// ebx
			unsigned uLargestLineSizeMONITOR : 16; // [bits 15:0]
			unsigned : 16;
			// ecx
			unsigned uEMX : 1; // bit 0: Enumerate MONITOR/MWAIT extensions
			unsigned uIBE : 1; // bit 1: Interrupt break-event
			unsigned : 30;
			// edx
			unsigned uNumberOfC0 : 4;
			unsigned uNumberOfC1 : 4;
			unsigned uNumberOfC2 : 4;
			unsigned uNumberOfC3 : 4;
			unsigned uNumberOfC4 : 4;
			unsigned uNumberOfC5 : 4;
			unsigned uNumberOfC6 : 4;
			unsigned uNumberOfC7 : 4;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x05> const & cpuid)
		{
			return os
				<< " [" << (cpuid.uSmallestLineSizeMONITOR) << "B," << (cpuid.uLargestLineSizeMONITOR) << "B]"
				<< (cpuid.uEMX ? " +EMX" : " -EMX")
				<< (cpuid.uIBE ? " +IBE" : " -IBE")
				<< ' ' << "C0/" << (cpuid.uNumberOfC0)
				<< ' ' << "C1/" << (cpuid.uNumberOfC1)
				<< ' ' << "C2/" << (cpuid.uNumberOfC2)
				<< ' ' << "C3/" << (cpuid.uNumberOfC3)
				<< ' ' << "C4/" << (cpuid.uNumberOfC4)
				<< ' ' << "C5/" << (cpuid.uNumberOfC5)
				<< ' ' << "C6/" << (cpuid.uNumberOfC6)
				<< ' ' << "C7/" << (cpuid.uNumberOfC7);
		}
		template <> struct cpuid_info_t<0x06>
		{
			// eax
			unsigned uDigitalThermalSensor : 1; // bit 0
			unsigned uIntelTurboBoostTechnology : 1; // bit 1
			unsigned uARAT : 1; // bit 2: APIC-Timer always-running feature is supported if set
			unsigned : 1;
			unsigned uPLN : 1; // bit 4: Power limit notification controls are supported if set
			unsigned uECMD : 1; // bit 5: Clock modulation duty cycle extension is supported if set
			unsigned uPTM : 1; // bit 6: Package thermal management is supported if set.
			unsigned uHWP : 1; // bit 7
			unsigned uHWP_Notification : 1; // bit 8
			unsigned uHWP_ActivityWindow : 1; // bit 9
			unsigned uHWP_EnergyPerformancePreference : 1; // bit 10
			unsigned uHWP_PackageLevelRequest : 1; // bit 11
			unsigned : 1;
			unsigned uHDC : 1; // bit 13
			unsigned : 18;
			// ebx
			unsigned uNumOfThermalThreshold : 4; // [bits 3:0]: Number of Interrupt Thresholds in Digital Thermal Sensor
			unsigned : 28;
			// ecx
			unsigned uPStateHardwareCoordination : 1; // bit 0
			unsigned : 2;
			unsigned uSETBH : 1; // bit 3
			unsigned : 28;
			// edx
			unsigned : 32;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x06> const & cpuid)
		{
			return os <<
				(cpuid.uDigitalThermalSensor ? " +ThermalSensor" : " -ThermalSensor") <<
				(cpuid.uIntelTurboBoostTechnology ? " +TurboBoost" : " -TurboBoost") <<
				(cpuid.uARAT ? " +ARAT" : " -ARAT") <<
				(cpuid.uPLN ? " +PLN" : " -PLN") <<
				(cpuid.uECMD ? " +ECMD" : " -ECMD") <<
				(cpuid.uPTM ? " +PTM" : " -PTM") <<
				(cpuid.uHWP ? " +HWP" : " -HWP") <<
				(cpuid.uHDC ? " +HDC" : " -HDC") <<
				(cpuid.uSETBH ? " +SETBH" : " -SETBH");
		}
		template <> struct cpuid_info_t<0x07>
		{
			// eax
			unsigned uMaxSubLeaf : 32;
			// ebx
			unsigned uFSGSBASE : 1; // bit 0
			unsigned uIA32_TSC_ADJUST : 1; // bit 1
			unsigned uIntelSGX : 1; // bit 2
			unsigned uBMI1 : 1; // bit 3
			unsigned uHLE : 1; // bit 4
			unsigned uAVX2 : 1; // bit 5
			unsigned uFDP_EXCPTN_ONLY : 1; // bit 6: x87 FPU Data Pointer updated only on x87 exceptions if 1
			unsigned uSMEP : 1; // bit 7: Supports Supervisor-Mode Execution Prevention if 1.
			unsigned uBMI2 : 1; // bit 8
			unsigned uFastStringOperation : 1; // bit 9: Supports Enhanced REP MOVSB/STOSB if 1.
			unsigned uINVPCID : 1; // bit 10: If 1, supports INVPCID instruction for system software that manages process-context identifiers.
			unsigned uRTM : 1; // bit 11
			unsigned uPQM : 1; // bit 12: Supports Platform Quality of Service Monitoring (PQM) capability if 1.
			unsigned uFCS_FDS : 1; // bit 13: suppress FCS, FDS
			unsigned uIntelMPX : 1; // bit 14
			unsigned uPQE : 1; // bit 15: Supports Platform Quality of Service Enforcement (PQE) capability if 1.
			unsigned uAVX512F : 1; // bit 16: avx-512 foundation
			unsigned uAVX512DQ : 1; // bit 17: avx-512 doubleword and quadword instructions
			unsigned uRDSEED : 1; // bit 18
			unsigned uADX : 1; // bit 19
			unsigned uSMAP : 1; // bit 20
			unsigned uAVX512IFMA : 1; // bit 21
			unsigned uPCOMMIT : 1; // bit 22
			unsigned uCLFLUSHOPT : 1; // bit 23
			unsigned uCLWB : 1;
			unsigned uIntelProcessorTrace : 1; // bit 25
			unsigned uAVX512PF : 1; // bit 26
			unsigned uAVX512ER : 1; // bit 27
			unsigned uAVX512CD : 1; // bit 28
			unsigned uSHA : 1; // bit 29
			unsigned uAVX512BW : 1; // bit 30
			unsigned uAVX512VL : 1; // bit 31
									// ecx
			unsigned uPREFTEHCHWT1 : 1; // bit 0
			unsigned uAVX512VBMI : 1; // bit 1
			unsigned : 1;
			unsigned uPKU : 1; // bit 3: Supports protection keys for user-mode pages if 1.
			unsigned uOSPKE : 1; // bit 4: If 1, OS has set CR4.PKE to enable protection keys (and the RDPKRU/WRPKRU instructions)
			unsigned : 27;
			// edx
			unsigned : 32;

			unsigned max_sub_leaf() const
			{
				return uMaxSubLeaf;
			}
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x07> const & cpuid)
		{
			return os <<
				(cpuid.uFSGSBASE ? " +FSGSBASE" : " -FSGSBASE") <<
				(cpuid.uIA32_TSC_ADJUST ? " +IA32_TSC_ADJUST" : " -IA32_TSC_ADJUST") <<
				(cpuid.uIntelSGX ? " +SGX" : " -SGX") <<
				(cpuid.uBMI1 ? " +BMI1" : " -BMI1") <<
				(cpuid.uHLE ? " +HLE" : " -HLE") <<
				(cpuid.uAVX2 ? " +AVX2" : " -AVX2") <<
				(cpuid.uFDP_EXCPTN_ONLY ? " +FDP_EXCPTN_ONLY" : " -FDP_EXCPTN_ONLY") <<
				(cpuid.uSMEP ? " +SMEP" : " -SMEP") <<
				(cpuid.uBMI2 ? " +BMI2" : " -BMI2") <<
				(cpuid.uFastStringOperation ? " +FastString" : " -FastString") <<
				(cpuid.uINVPCID ? " +INVPCID" : " -INVPCID") <<
				(cpuid.uRTM ? " +RTM" : " -RTM") <<
				(cpuid.uPQM ? " +PQM" : " -PQM") <<
				(cpuid.uFCS_FDS ? " +SuppressFCS +SuppressFDS" : " -SuppressFCS -SuppressFDS") <<
				(cpuid.uIntelMPX ? " +MPX" : " -MPX") <<
				(cpuid.uPQE ? " +PQE" : " -PQE") <<
				(cpuid.uAVX512F ? " +AVX512F" : " -AVX512F") << // bit 16: avx-512 foundation
				(cpuid.uAVX512DQ ? " +AVX512DQ" : " -AVX512DQ") << // bit 17: avx-512 doubleword and quadword instructions
				(cpuid.uRDSEED ? " +RDSEED" : " -RDSEED") <<
				(cpuid.uADX ? " +ADX" : " -ADX") <<
				(cpuid.uSMAP ? " +SMAP" : " -SMAP") <<
				(cpuid.uAVX512IFMA ? " +AVX512IFMA" : " -AVX512IFMA") << // bit 21
				(cpuid.uPCOMMIT ? " +PCOMMIT" : " -PCOMMIT") <<  // bit 22
				(cpuid.uCLFLUSHOPT ? " +CLFLUSHOPT" : " -CLFLUSHOPT") <<
				(cpuid.uCLWB ? " +CLWB" : " -CLWB") <<
				(cpuid.uIntelProcessorTrace ? " +PT" : " -PT") <<
				(cpuid.uAVX512PF ? " +AVX512PF" : " -AVX512PF") <<
				(cpuid.uAVX512ER ? " +AVX512ER" : " -AVX512ER") <<
				(cpuid.uAVX512CD ? " +AVX512CD" : " -AVX512CD") <<
				(cpuid.uSHA ? " +SHA" : " -SHA") << // bit 29
				(cpuid.uAVX512BW ? " +AVX512BW" : " -AVX512BW") <<
				(cpuid.uAVX512VL ? " +AVX512VL" : " -AVX512VL") <<
				(cpuid.uPREFTEHCHWT1 ? " +PREFTEHCHWT1" : " -PREFTEHCHWT1") <<
				(cpuid.uAVX512VBMI ? " +AVX512VBMI" : " -AVX512VBMI") <<
				(cpuid.uPKU ? " +PKU" : " -PKU") <<
				(cpuid.uOSPKE ? " +OSPKE" : " -OSPKE");
		}
		template <> struct cpuid_info_t<0x09>
		{
			// eax
			unsigned uIA32_PLATFORM_DCA_CAP : 32;
			// ebx
			unsigned : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 32;
		};
		template <> struct cpuid_info_t<0x0A>
		{
			// eax
			unsigned uVersionID : 8; // [bits 7:0]: Version ID of architectural performance monitoring
			unsigned uNumOfGP : 8; // [bits 15:8]: Number of general-purpose performance monitoring counter per logical processor
			unsigned uBitWidthOfGP : 8; // [bits 23:16]: Bit width of general-purpose, performance monitoring counter
			unsigned uLengethOfEBX : 8; // [bits 31:24]: Length of EBX bit vector to enumerate architectural performance monitoring events
										// ebx
			unsigned uCoreCycleEventUnavailable : 1; // bit 0
			unsigned uInstructionRetiredEventUnavailable : 1; // bit 1
			unsigned uReferenceCyclesEventUnavailable : 1; // bit 2
			unsigned uLLC_ReferenceEventUnavailable : 1; // bit 3
			unsigned uLLC_MissesEventUnavailable : 1; // bit 4
			unsigned uBranchInstructionRetiredEventUnavailable : 1; // bit 5
			unsigned uBranchMispredictRetiredEventUnavailable : 1; // bit 6
			unsigned : 25;
			// ecx
			unsigned : 32;
			// edx
			unsigned uNumOfFixedFunctionPerformanceCounters : 5; // [bits 4:0]
			unsigned uBitWidthOfFixedFunctionPerformanceCounters : 8; // [bits 12:5]
			unsigned : 19;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0A> const & cpuid)
		{
			os
				<< " VersionID=" << cpuid.uVersionID
				<< ' ' << (cpuid.uNumOfGP) << '*' << (cpuid.uBitWidthOfGP) << 'b';

			if (cpuid.uLengethOfEBX > 0) os << (cpuid.uCoreCycleEventUnavailable ? " -CoreCycleEvent" : " +CoreCycleEvent");
			if (cpuid.uLengethOfEBX > 1) os << (cpuid.uInstructionRetiredEventUnavailable ? " -InstructionRetiredEvent" : " +InstructionRetiredEvent");
			if (cpuid.uLengethOfEBX > 2) os << (cpuid.uReferenceCyclesEventUnavailable ? " -ReferenceCyclesEvent" : " +ReferenceCyclesEvent");
			if (cpuid.uLengethOfEBX > 3) os << (cpuid.uLLC_ReferenceEventUnavailable ? " -LLC_ReferenceEvent" : " +LLC_ReferenceEvent");
			if (cpuid.uLengethOfEBX > 4) os << (cpuid.uLLC_MissesEventUnavailable ? " -LLC_MissesEvent" : " +LLC_MissesEvent");
			if (cpuid.uLengethOfEBX > 5) os << (cpuid.uBranchInstructionRetiredEventUnavailable ? " -BranchInstructionRetiredEvent" : " +BranchInstructionRetiredEvent");
			if (cpuid.uLengethOfEBX > 6) os << (cpuid.uBranchMispredictRetiredEventUnavailable ? " -BranchMispredictRetiredEvent" : " +BranchMispredictRetiredEvent");

			if (cpuid.uVersionID > 1)
			{
				os << ' ' << (cpuid.uNumOfFixedFunctionPerformanceCounters) << '*' << (cpuid.uBitWidthOfFixedFunctionPerformanceCounters) << 'b' << ' ';
			}
			return os;
		}
		template <unsigned uECX> struct cpuid_info_t<0x0B, uECX>
		{
			// eax
			unsigned uNumOfBitsToShift : 5; // [bits 4:0]
			unsigned : 27;
			// ebx
			unsigned uNumOfLogicalProcessors : 16;
			unsigned : 16;
			// ecx
			unsigned uLevelNumber : 8; // [bits 7:0]
			unsigned uLevelType : 8; // [bits 15:8]
			unsigned : 16;
			// edx
			unsigned u_x2APIC_ID : 32;

			unsigned max_sub_leaf() const
			{
				return ~0U;
			}
			bool is_sub_leaf(unsigned) const
			{
				return uLevelType != 0;
			}
		};
		template <unsigned uECX> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0B, uECX> const & cpuid)
		{
			switch (cpuid.uLevelType)
			{
			case 0:
				os << " Invalid";
				return os;
			case 1:
				os << std::setw(8) << " SMT";
				break;
			case 2:
				os << std::setw(8) << " Core";
				break;
			default:
				os << std::setw(8) << " Reserved";
				break;
			}
			os << " x2APIC-ID=" << cpuid.u_x2APIC_ID;
			os << " Topology-ID=" << (cpuid.u_x2APIC_ID >> cpuid.uNumOfBitsToShift);
			os << " Logical-Processor=" << (cpuid.uNumOfLogicalProcessors);
			return os;
		}
		template <> struct cpuid_info_t<0x0D>
		{
			// eax
			unsigned uStateX87 : 1; // bit 0
			unsigned uStateSSE : 1; // bit 1
			unsigned uStateAVX : 1;
			unsigned uStateBNDREGS : 1; // bit 3
			unsigned uStateBNDCSR : 1; // bit 4
			unsigned uStateOpmask : 1; // bit 5
			unsigned uStateZMM_Hi256 : 1; // bit 6
			unsigned uStateHi16_ZMM : 1; // bit 7
			unsigned uStatePT : 1; // bit 8
			unsigned uStatePKRU : 1; // bit 9
			unsigned : 22;
			// ebx
			unsigned uSizeXCR0 : 32;
			// ecx
			unsigned uSize : 32;
			// edx
			unsigned : 30;
			unsigned uStateLWP : 1; // bit 30: Lightweight Profiling
			unsigned : 1;

			unsigned max_sub_leaf() const
			{
				return _lzcnt_u32(reinterpret_cast<unsigned const*>(this)[0]);
			}
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D> const & cpuid)
		{
			return os <<
				(cpuid.uStateX87 ? " +x87" : " -x87") <<
				(cpuid.uStateSSE ? " +SSE" : " -SSE") <<
				(cpuid.uStateAVX ? " +AVX" : " -AVX") <<
				(cpuid.uStateBNDREGS ? " +BNDREGS" : " -BNDREGS") <<
				(cpuid.uStateBNDCSR ? " +BNDCSR" : " -BNDCSR") <<
				(cpuid.uStateOpmask ? " +Opmask" : " -Opmask") <<
				(cpuid.uStateZMM_Hi256 ? " +ZMM_Hi256" : " -ZMM_Hi256") <<
				(cpuid.uStateHi16_ZMM ? " +Hi16_ZMM" : " -Hi16_ZMM") <<
				(cpuid.uStatePT ? " +PT" : " -PT") <<
				(cpuid.uStatePKRU ? " +PKRU" : " -PKRU") <<
				(cpuid.uStateLWP ? " +LWP" : " -LWP") <<
				' ' << (cpuid.uSizeXCR0) << 'B' <<
				' ' << (cpuid.uSize) << 'B';
		}
		template <> struct cpuid_info_t<0x0D, 1>
		{
			// eax
			unsigned uXSAVEOPT : 1; // bit 0
			unsigned uCompactionExtensions : 1; // bit 1
			unsigned uXGETBV : 1; // bit 2
			unsigned uXSAVES_IA32_XSS : 1; // bit 3
			unsigned : 28;
			// ebx
			unsigned uSize : 32;
			// ecx
			// edx
			unsigned long long uBitmap;

			bool is_sub_leaf(unsigned) const
			{
				return true;
			}
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 1> const & cpuid)
		{
			return os <<
				(cpuid.uXSAVEOPT ? " +XSAVEOPT" : " -XSAVEOPT") <<
				(cpuid.uCompactionExtensions ? " +Compaction_Extensions" : " -Compaction_Extensions") <<
				(cpuid.uXGETBV ? " +XGETBV" : " -XGETBV") <<
				(cpuid.uXSAVES_IA32_XSS ? " +XSAVES_IA32_XSS" : " -XSAVES_IA32_XSS") <<
				' ' << (cpuid.uSize) << 'B';
		}
		template <unsigned uECX> struct cpuid_info_t<0x0D, uECX> // AVX state, BNGREG state, BNDCSR state, Opmask state, ZMM_Hi256 state, Hi16_ZMM state, PT state, PKRU state
		{
			// eax
			unsigned uSize;
			// ebx
			unsigned uOffset;
			// ecx
			unsigned uIA32_XSS : 1; // bit 0
			unsigned uAlign64 : 1; // bit 1
			unsigned : 30;
			// edx
			unsigned : 32;

			bool is_sub_leaf(unsigned uMaxECX) const
			{
				return static_cast<unsigned>(uECX) <= uMaxECX;
			}
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 2> const & cpuid) { return os << " AVX       " << std::setw(4) << cpuid.uSize << 'B' << " @ " << std::setw(4) << cpuid.uOffset << 'B'; }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 3> const & cpuid) { return os << " BNGREG    " << std::setw(4) << cpuid.uSize << 'B' << " @ " << std::setw(4) << cpuid.uOffset << 'B'; }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 4> const & cpuid) { return os << " BNGCSR    " << std::setw(4) << cpuid.uSize << 'B' << " @ " << std::setw(4) << cpuid.uOffset << 'B'; }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 5> const & cpuid) { return os << " Opmask    " << std::setw(4) << cpuid.uSize << 'B' << " @ " << std::setw(4) << cpuid.uOffset << 'B'; }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 6> const & cpuid) { return os << " ZMM_Hi256 " << std::setw(4) << cpuid.uSize << 'B' << " @ " << std::setw(4) << cpuid.uOffset << 'B'; }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 7> const & cpuid) { return os << " Hi16_ZMM  " << std::setw(4) << cpuid.uSize << 'B' << " @ " << std::setw(4) << cpuid.uOffset << 'B'; }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 8> const & cpuid) { return os << " PT        " << std::setw(4) << cpuid.uSize << 'B' << " @ " << std::setw(4) << cpuid.uOffset << 'B'; }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x0D, 9> const & cpuid) { return os << " PKRU      " << std::setw(4) << cpuid.uSize << 'B' << " @ " << std::setw(4) << cpuid.uOffset << 'B'; }

		template <> struct cpuid_info_t<0x0F>
		{
			// eax
			unsigned : 32;
			// ebx
			unsigned uMaxOfRMID : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 1;
			unsigned uL3CacheQoS_Monitoring : 1; // bit 1
			unsigned : 30;

			unsigned max_sub_leaf() const
			{
				return _lzcnt_u32(reinterpret_cast<unsigned const*>(this)[3]);
			}
		};
		template <> struct cpuid_info_t<0x0F, 1>
		{
			// eax
			unsigned : 32;
			// ebx
			unsigned uConversionFactor : 32;
			// ecx
			unsigned uMaxOfRMID : 32;
			// edx
			unsigned uL3OccupancyMonitoring : 1; // bit 0
			unsigned : 31;

			bool is_sub_leaf(unsigned uMaxECX) const
			{
				return 1 <= uMaxECX;
			}
		};
		template <> struct cpuid_info_t<0x10>
		{
			// eax
			unsigned : 32;
			// ebx
			unsigned : 1;
			unsigned uL3CacheQoS_Enforcement : 1; // bit 1
			unsigned : 30;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 32;

			unsigned max_sub_leaf() const
			{
				return _lzcnt_u32(reinterpret_cast<unsigned const*>(this)[1]);
			}
		};
		template <> struct cpuid_info_t<0x10, 1>
		{
			// eax
			unsigned uNumOfCapacity : 5; // [bits 4:0]
			unsigned : 27;
			// ebx
			unsigned uAllocationUnitsBitmap : 32;
			// ecx
			unsigned : 1;
			unsigned uInfrequentUpdateOfCOS : 1; // bit 1
			unsigned uPrioritizationTechnology : 1; // bit 2
			unsigned : 29;
			// edx
			unsigned uMaxOfCOS : 16; // [bits 15:0]
			unsigned : 16;

			bool is_sub_leaf(unsigned uMaxECX) const
			{
				return 1 <= uMaxECX;
			}
		};
		template <> struct cpuid_info_t<0x12>
		{
			// eax
			unsigned uSGX1 : 1; // bit 0
			unsigned uSGX2 : 1; // bit 1
			unsigned : 30;
			// ebx
			unsigned uMISCSELECT : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned uMaxEnclaveSize_Not64 : 8; // [7:0]
			unsigned uMaxEnclaveSize_64 : 8; // [15:8]
			unsigned : 16;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x12> const & cpuid)
		{
			return os <<
				(cpuid.uSGX1 ? " +SGX1" : " -SGX1") <<
				(cpuid.uSGX2 ? " +SGX2" : " -SGX2");
		}
		template <> struct cpuid_info_t<0x14>
		{
			// eax
			unsigned uMaxSubLeaf : 32;
			// ebx
			unsigned uIA32_RTIT_CR3 : 1; // bit 0: IA32_RTIT_CTL.CR3Filter can be set to 1, and that IA32_RTIT_CR3_MATCH MSR can be accessed
			unsigned uConfigurablePSB : 1; // bit 1: Configurable PSB and Cycle-Accurate Mode.
			unsigned uIntelPT : 1; // bit 2: IP Filtering, TraceStop filtering, and preservation of Intel PT MSRs across warm reset.
			unsigned uMTC : 1; // bit 3: MTC timing packet and suppression of COFI-based packets
			unsigned : 28;
			// ecx
			unsigned uIA32_RTIT_CTL_ToPA : 1; // bit 0
			unsigned uIA32_RTIT_OUTPUT_MASK_PTRS : 1; // bit 1
			unsigned uSingleRangeOutputScheme : 1; // bit 2
			unsigned uOutputToTraceTransportSubsystem : 1; // bit 3
			unsigned : 27;
			unsigned uLIP : 1; // bit 31: Generated packets which contain IP payloads have LIP values, which include the CS base component.
							   // edx
			unsigned : 32;

			unsigned max_sub_leaf() const
			{
				return uMaxSubLeaf;
			}
		};
		template <> struct cpuid_info_t<0x14, 1>
		{
			// eax
			unsigned uNumOfConfigurableAddressRanges : 3; // [bits 2:0]
			unsigned : 13;
			unsigned uMTC_PeriodEncodingsBitmap : 16; // [bits 31:16]
													  // ebx
			unsigned uCycleThresholdValueEncodingsBitmap : 16; // [bits 15:0]
			unsigned uConfigurablePSB_FrequencyEncodingsBitmap : 16; // [bits 31:16]
																	 // ecx
			unsigned : 32;
			// edx
			unsigned : 32;

			bool is_sub_leaf(unsigned uMaxECX) const
			{
				return 1 <= uMaxECX;
			}
		};
		template <> struct cpuid_info_t<0x15>
		{
			// eax
			unsigned uCoreCrystalClock : 32;
			// ebx
			unsigned uTSC : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 32;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x15> const & cpuid)
		{
			return os << ' ' << cpuid.uTSC << "TSC / " << cpuid.uCoreCrystalClock << "CLK";
		}
		template <> struct cpuid_info_t<0x16>
		{
			// eax
			unsigned uProcessorBaseFrequencyMHz : 16; // [bits 15:0]
			unsigned : 16;
			// ebx
			unsigned uMaximumFrequencyMHz : 16; // [bits 15:0]
			unsigned : 16;
			// ecx
			unsigned uBusFrequencyMHz : 16; // [bits 15:0]
			unsigned : 16;
			// edx
			unsigned : 32;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x16> const & cpuid)
		{
			return os
				<< " [" << cpuid.uProcessorBaseFrequencyMHz << "MHz," << cpuid.uMaximumFrequencyMHz << "MHz]"
				<< '@' << cpuid.uBusFrequencyMHz << "MHz";
		}
		template <> struct cpuid_info_t<0x17>
		{
			// eax
			unsigned uMaxSOCID_Index : 32;
			// ebx
			unsigned uSOC_VendorID : 16;
			unsigned uIsVendorScheme : 1;
			unsigned : 15;
			// ecx
			unsigned uProjectID : 32;
			// edx
			unsigned uSteppingID : 32;

			unsigned max_sub_leaf() const
			{
				return uMaxSOCID_Index;
			}
		};
		template <> struct cpuid_info_t<0x17, 1>
		{
			char ac[16];

			char const * soc_vendor_brand_string(void) const
			{
				return ac;
			}
			bool is_sub_leaf(unsigned uMaxECX) const
			{
				return 1 <= uMaxECX;
			}
		};
		template <> struct cpuid_info_t<0x17, 2>
		{
			char ac[16];

			bool is_sub_leaf(unsigned uMaxECX) const
			{
				return 2 <= uMaxECX;
			}
		};
		template <> struct cpuid_info_t<0x17, 3>
		{
			char ac[16];

			bool is_sub_leaf(unsigned uMaxECX) const
			{
				return 3 <= uMaxECX;
			}
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x17, 1> const & cpuid) { return (os << ' ').write(cpuid.ac, 16); }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x17, 2> const & cpuid) { return (os << ' ').write(cpuid.ac, 16); }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x17, 3> const & cpuid) { return (os << ' ').write(cpuid.ac, 16); }

		template <> struct cpuid_info_t<0x40000000>
		{
			// eax
			unsigned uMaxLeaf : 32;
			// ebx
			// ecx
			// edx
			char acHypervisorVendorIdSignature[12];

			unsigned max_leaf() const
			{
				return uMaxLeaf;
			}
		};
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x40000000> const & cpuid)
		{
			return (os << ' ').write(cpuid.acHypervisorVendorIdSignature, 12);
		}
		template <> struct cpuid_info_t<0x40000001>
		{
			// eax
			char acHypervisorInterfaceSignature[4];
			// ebx
			unsigned : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 32;
		};
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x40000001> const & cpuid)
		{
			return (os << ' ').write(cpuid.acHypervisorInterfaceSignature, 4);
		}
		template <> struct cpuid_info_t<0x40000002>
		{
			// eax
			unsigned uBuildNumber : 32;
			// ebx
			unsigned uMinorVersion : 16;
			unsigned uMajorVersion : 16;
			// ecx
			unsigned uServicePack : 32;
			// edx
			unsigned uServiceNumber : 24;
			unsigned uServiceBranch : 8;
		};
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x40000002> const & cpuid)
		{
			return os
				<< " [Version " << cpuid.uMajorVersion << '.' << cpuid.uMinorVersion << '.' << cpuid.uBuildNumber << ']'
				<< " SP" << cpuid.uServicePack << '.' << cpuid.uServiceNumber << '.' << cpuid.uServiceBranch
				;
		}
		template <> struct cpuid_info_t<0x40000003>
		{
			// eax
			//Bit 0: VP Runtime(HV_X64_MSR_VP_RUNTIME) available
			unsigned uVP_Runtime : 1;
			//Bit 1 : Partition Reference Counter(HV_X64_MSR_TIME_REF_COUNT) available
			unsigned uPartitionReferenceCounter : 1;
			//Bit 2 : Basic SynIC MSRs(HV_X64_MSR_SCONTROL through HV_X64_MSR_EOM and HV_X64_MSR_SINT0 through HV_X64_MSR_SINT15) available
			unsigned uBasicSynIC_MSRs : 1;
			//Bit 3 : Synthetic Timer MSRs(HV_X64_MSR_STIMER0_CONFIG through HV_X64_MSR_STIMER3_COUNT) available
			unsigned uSyntheticTimerMSRs : 1;
			//Bit 4 : APIC access MSRs(HV_X64_MSR_EOI, HV_X64_MSR_ICR and HV_X64_MSR_TPR) are available
			unsigned uAPIC_AccessMSRs : 1;
			//Bit 5 : Hypercall MSRs(HV_X64_MSR_GUEST_OS_ID and HV_X64_MSR_HYPERCALL) available
			unsigned uHypercallMSRs : 1;
			//Bit 6 : Access virtual processor index MSR(HV_X64_MSR_VP_INDEX) available
			unsigned uAccessVirtualProcessorIndexMSRs : 1;
			//Bit 7 : Virtual system reset MSR(HV_X64_MSR_RESET) is available.
			unsigned uVirtualSystemResetMSR : 1;
			//Bit 8 : Access statistics pages MSRs(HV_X64_MSR_STATS_PARTITION_RETAIL_PAGE, HV_X64_MSR_STATS_PARTITION_INTERNAL_PAGE, HV_X64_MSR_STATS_VP_RETAIL_PAGE, HV_X64_MSR_STATS_VP_INTERNAL_PAGE) available.
			unsigned uAccessStatisticsPagesMSRs : 1;
			//Bit 9 : Partition Reference TSC MSR(HV_X64_MSR_REFERENCE_TSC) available.
			unsigned uAccessPartitionReferenceTSC : 1;
			//Bit 10 : Virtual Guest Idle State MSR(HV_X64_MSR_GUEST_IDLE) available.
			unsigned uAccessGuestIdleMSRs : 1;
			//Bit 11 : Timer frequency MSRs(HV_X64_MSR_TSC_FREQUENCY and HV_X64_MSR_APIC_FREQUENCY) available.
			unsigned uAccessFrequencyMSRs : 1;
			//Bit 12 : Debug MSRs(HV_X64_MSR_SYNTH_DEBUG_CONTROL, HV_X64_MSR_SYNTH_DEBUG_STATUS, HV_X64_MSR_SYNTH_DEBUG_SEND_BUFFER, HV_X64_MSR_SYNTH_DEBUG_RECEIVE_BUFFER, HV_X64_MSR_SYNTH_DEBUG_PENDING_BUFFER) available
			unsigned uAccessDebugMSRs : 1;
			//Bits 13 - 31 : Reserved
			unsigned : 19;
			// ebx
			//Bit 0: CreatePartitions
			unsigned uCreatePartitions : 1;
			//Bit 1 : AccessPartitionId
			unsigned uAccessPartitionId : 1;
			//Bit 2 : AccessMemoryPool
			unsigned uAccessMemoryPool : 1;
			//Bit 3 : AdjustMessageBuffers
			unsigned uAdjustMessageBuffers : 1;
			//Bit 4 : PostMessages
			unsigned uPostMessages : 1;
			//Bit 5 : SignalEvents
			unsigned uSignalEvents : 1;
			//Bit 6 : CreatePort
			unsigned uCreatePort : 1;
			//Bit 7 : ConnectPort
			unsigned uConnectPort : 1;
			//Bit 8 : AccessStats
			unsigned uAccessStats : 1;
			//Bit 9 - 10 : RsvdZ
			unsigned : 2;
			//Bit 11 : Debugging
			unsigned uDebugging : 1;
			//Bit 12 : CpuManagement
			unsigned uCpuManagement : 1;
			//Bit 13 : ConfigureProfiler
			unsigned uConfigureProfiler : 1;
			//Bit 14 : EnableExpandedStackwalking
			unsigned uEnableExpandedStackwalking : 1;
			//Bit 15 - 31 : Reserved
			unsigned : 17;
			// ecx
			//Bit 0 - 3: Maximum Processor Power State. 0 = C0, 1 = C1, 2 = C2, 3 = C3
			enum power_state : unsigned
			{
				C0 = 0,
				C1 = 1,
				C2 = 2,
				C3 = 3,
			} emMaximumProcessorPowerState : 4;
			//Bit 4 : HPET is required to enter C3
			unsigned uHPET : 1;
			//Bits 5 - 31 : Reserved
			unsigned : 27;
			// edx
			//Bit 0: The MWAIT instruction is available(per section 11.5)
			unsigned uMWAIT : 1;
			//Bit 1 : Guest debugging support is available
			unsigned uGuestDebugging : 1;
			//Bit 2 : Performance Monitor support is available
			unsigned uPerformanceMonitor : 1;
			//Bit 3 : Support for physical CPU dynamic partitioning events is available
			unsigned uPhysicalCPU_DynamicPartitioningEvents : 1;
			//Bit 4 : Support for passing hypercall input parameter block via XMM registers is available
			unsigned uHypercallInputViaXMM : 1;
			//Bit 5 : Support for a virtual guest idle state is available
			unsigned uVirtualGuestIdleState : 1;
			//Bit 6 : Support for hypervisor sleep state is available.
			unsigned uHypervisorSleepState : 1;
			//Bit 7 : Support for querying NUMA distances is available.
			unsigned uNUMA_Distances : 1;
			//Bit 8 : Support for determining timer frequencies is available.
			unsigned uTimerFrequencies : 1;
			//Bit 9 : Support for injecting synthetic machine checks is available.
			unsigned uSyntheticMachineChecks : 1;
			//Bit 10 : Support for guest crash MSRs is available.
			unsigned uGuestCrashMSRs : 1;
			//Bit 11 : Support for debug MSRs is available.
			unsigned uDebugMSRs : 1;
			//Bit 12 : Support for NPIEP is available
			unsigned uNPIEP : 1;
			//Bit 13 : Support for disabling the hypervisor is available
			unsigned uDisableHypervisor : 1;
			//Bits 14 - 31 : Reserved
			unsigned : 18;
		};
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x40000003>::power_state const & emPowerState)
		{
			return os << 'C' << ((int) emPowerState);
		}
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x40000003> const & cpuid)
		{
			return os
				// eax
				//Bit 0: VP Runtime(HV_X64_MSR_VP_RUNTIME) available
				<< (cpuid.uVP_Runtime ? " +VP_Runtime" : " -VP_Runtime")
				//Bit 1 : Partition Reference Counter(HV_X64_MSR_TIME_REF_COUNT) available
				<< (cpuid.uPartitionReferenceCounter ? " +PartitionReferenceCounter" : " -PartitionReferenceCounter")
				//Bit 2 : Basic SynIC MSRs(HV_X64_MSR_SCONTROL through HV_X64_MSR_EOM and HV_X64_MSR_SINT0 through HV_X64_MSR_SINT15) available
				<< (cpuid.uBasicSynIC_MSRs ? " +BasicSynIC_MSRs" : " -BasicSynIC_MSRs")
				//Bit 3 : Synthetic Timer MSRs(HV_X64_MSR_STIMER0_CONFIG through HV_X64_MSR_STIMER3_COUNT) available
				<< (cpuid.uSyntheticTimerMSRs ? " +SyntheticTimerMSRs" : " -SyntheticTimerMSRs")
				//Bit 4 : APIC access MSRs(HV_X64_MSR_EOI, HV_X64_MSR_ICR and HV_X64_MSR_TPR) are available
				<< (cpuid.uAPIC_AccessMSRs ? " +APIC_AccessMSRs" : " -APIC_AccessMSRs")
				//Bit 5 : Hypercall MSRs(HV_X64_MSR_GUEST_OS_ID and HV_X64_MSR_HYPERCALL) available
				<< (cpuid.uHypercallMSRs ? " +HypercallMSRs" : " -HypercallMSRs")
				//Bit 6 : Access virtual processor index MSR(HV_X64_MSR_VP_INDEX) available
				<< (cpuid.uAccessVirtualProcessorIndexMSRs ? " +AccessVirtualProcessorIndexMSRs" : " -AccessVirtualProcessorIndexMSRs")
				//Bit 7 : Virtual system reset MSR(HV_X64_MSR_RESET) is available.
				<< (cpuid.uVirtualSystemResetMSR ? " +VirtualSystemResetMSR" : " -VirtualSystemResetMSR")
				//Bit 8 : Access statistics pages MSRs(HV_X64_MSR_STATS_PARTITION_RETAIL_PAGE, HV_X64_MSR_STATS_PARTITION_INTERNAL_PAGE, HV_X64_MSR_STATS_VP_RETAIL_PAGE, HV_X64_MSR_STATS_VP_INTERNAL_PAGE) available.
				<< (cpuid.uAccessStatisticsPagesMSRs ? " +AccessStatisticsPagesMSRs" : " -AccessStatisticsPagesMSRs")
				//Bit 9 : Partition Reference TSC MSR(HV_X64_MSR_REFERENCE_TSC) available.
				<< (cpuid.uAccessPartitionReferenceTSC ? " +AccessPartitionReferenceTSC" : " -AccessPartitionReferenceTSC")
				//Bit 10 : Virtual Guest Idle State MSR(HV_X64_MSR_GUEST_IDLE) available.
				<< (cpuid.uAccessGuestIdleMSRs ? " +AccessGuestIdleMSRs" : " -AccessGuestIdleMSRs")
				//Bit 11 : Timer frequency MSRs(HV_X64_MSR_TSC_FREQUENCY and HV_X64_MSR_APIC_FREQUENCY) available.
				<< (cpuid.uAccessFrequencyMSRs ? " +AccessFrequencyMSRs" : " -AccessFrequencyMSRs")
				//Bit 12 : Debug MSRs(HV_X64_MSR_SYNTH_DEBUG_CONTROL, HV_X64_MSR_SYNTH_DEBUG_STATUS, HV_X64_MSR_SYNTH_DEBUG_SEND_BUFFER, HV_X64_MSR_SYNTH_DEBUG_RECEIVE_BUFFER, HV_X64_MSR_SYNTH_DEBUG_PENDING_BUFFER) available
				<< (cpuid.uAccessDebugMSRs ? " +AccessDebugMSRs" : " -AccessDebugMSRs")
				//Bits 13 - 31 : Reserved
				// ebx
				//Bit 0: CreatePartitions
				<< (cpuid.uCreatePartitions ? " +CreatePartitions" : " -CreatePartitions")
				//Bit 1 : AccessPartitionId
				<< (cpuid.uAccessPartitionId ? " +AccessPartitionId" : " -AccessPartitionId")
				//Bit 2 : AccessMemoryPool
				<< (cpuid.uAccessMemoryPool ? " +AccessMemoryPool" : " -AccessMemoryPool")
				//Bit 3 : AdjustMessageBuffers
				<< (cpuid.uAdjustMessageBuffers ? " +AdjustMessageBuffers" : " -AdjustMessageBuffers")
				//Bit 4 : PostMessages
				<< (cpuid.uPostMessages ? " +PostMessages" : " -PostMessages")
				//Bit 5 : SignalEvents
				<< (cpuid.uSignalEvents ? " +SignalEvents" : " -SignalEvents")
				//Bit 6 : CreatePort
				<< (cpuid.uCreatePort ? " +CreatePort" : " -CreatePort")
				//Bit 7 : ConnectPort
				<< (cpuid.uConnectPort ? " +ConnectPort" : " -ConnectPort")
				//Bit 8 : AccessStats
				<< (cpuid.uAccessStats ? " +AccessStats" : " -AccessStats")
				//Bit 9 - 10 : RsvdZ
				//Bit 11 : Debugging
				<< (cpuid.uDebugging ? " +Debugging" : " -Debugging")
				//Bit 12 : CpuManagement
				<< (cpuid.uCpuManagement ? " +CpuManagement" : " -CpuManagement")
				//Bit 13 : ConfigureProfiler
				<< (cpuid.uConfigureProfiler ? " +ConfigureProfiler" : " -ConfigureProfiler")
				//Bit 14 : EnableExpandedStackwalking
				<< (cpuid.uEnableExpandedStackwalking ? " +EnableExpandedStackwalking" : " -EnableExpandedStackwalking")
				//Bit 15 - 31 : Reserved
				// ecx
				//Bit 0 - 3: Maximum Processor Power State. 0 = C0, 1 = C1, 2 = C2, 3 = C3
				<< ' ' << (cpuid.emMaximumProcessorPowerState)
				//Bit 4 : HPET is required to enter C3
				<< (cpuid.uHPET ? " +HPET" : " -HPET")
				//Bits 5 - 31 : Reserved
				// edx
				//Bit 0: The MWAIT instruction is available(per section 11.5)
				<< (cpuid.uMWAIT ? " +MWAIT" : " -MWAIT")
				//Bit 1 : Guest debugging support is available
				<< (cpuid.uGuestDebugging ? " +GuestDebugging" : " -GuestDebugging")
				//Bit 2 : Performance Monitor support is available
				<< (cpuid.uPerformanceMonitor ? " +PerformanceMonitor" : " -PerformanceMonitor")
				//Bit 3 : Support for physical CPU dynamic partitioning events is available
				<< (cpuid.uPhysicalCPU_DynamicPartitioningEvents ? " +PhysicalCPU_DynamicPartitioningEvents" : " -PhysicalCPU_DynamicPartitioningEvents")
				//Bit 4 : Support for passing hypercall input parameter block via XMM registers is available
				<< (cpuid.uHypercallInputViaXMM ? " +HypercallInputViaXMM" : " -HypercallInputViaXMM")
				//Bit 5 : Support for a virtual guest idle state is available
				<< (cpuid.uVirtualGuestIdleState ? " +VirtualGuestIdleState" : " -VirtualGuestIdleState")
				//Bit 6 : Support for hypervisor sleep state is available.
				<< (cpuid.uHypervisorSleepState ? " +HypervisorSleepState" : " -HypervisorSleepState")
				//Bit 7 : Support for querying NUMA distances is available.
				<< (cpuid.uNUMA_Distances ? " +NUMA_Distances" : " -NUMA_Distances")
				//Bit 8 : Support for determining timer frequencies is available.
				<< (cpuid.uTimerFrequencies ? " +TimerFrequencies" : " -TimerFrequencies")
				//Bit 9 : Support for injecting synthetic machine checks is available.
				<< (cpuid.uSyntheticMachineChecks ? " +SyntheticMachineChecks" : " -SyntheticMachineChecks")
				//Bit 10 : Support for guest crash MSRs is available.
				<< (cpuid.uGuestCrashMSRs ? " +GuestCrashMSRs" : " -GuestCrashMSRs")
				//Bit 11 : Support for debug MSRs is available.
				<< (cpuid.uDebugMSRs ? " +DebugMSRs" : " -DebugMSRs")
				//Bit 12 : Support for NPIEP is available
				<< (cpuid.uNPIEP ? " +NPIEP" : " -NPIEP")
				//Bit 13 : Support for disabling the hypervisor is available
				<< (cpuid.uDisableHypervisor ? " +DisableHypervisor" : " -DisableHypervisor")
				//Bits 14 - 31 : Reserved
				;
		}
		template <> struct cpuid_info_t<0x40000004>
		{
			// eax
			//Bit 0: Recommend using hypercall for address space switches rather than MOV to CR3 instruction
			unsigned uRecommendAddressSpaceSwitch : 1;
			//Bit 1 : Recommend using hypercall for local TLB flushes rather than INVLPG or MOV to CR3 instructions
			unsigned uRecommendLocalTLB_Flushes : 1;
			//Bit 2 : Recommend using hypercall for remote TLB flushes rather than inter - processor interrupts
			unsigned uRecommendRemoteTLB_Flushes : 1;
			//Bit 3 : Recommend using MSRs for accessing APIC registers EOI, ICR and TPR rather than their memory - mapped counterparts.
			unsigned uRecommendMSR_ForAPIC_Registers : 1;
			//Bit 4 : Recommend using the hypervisor - provided MSR to initiate a system RESET.
			unsigned uRecommendMSR_ToInitiateSystemRESET : 1;
			//Bit 5 : Recommend using relaxed timing for this partition.If used, the VM should disable any watchdog timeouts that rely on the timely delivery of external interrupts.
			unsigned uRecommendRelaxedTiming : 1;
			//Bit 6 : Recommend using DMA remapping.
			unsigned uUseDMA_Remapping : 1;
			//Bit 7 : Recommend using interrupt remapping.
			unsigned uUseInterruptRemapping : 1;
			//Bit 8 : Recommend using x2APIC MSRs.
			unsigned uUse_x2APIC_MSRs : 1;
			//Bit 9 : Recommend deprecating AutoEOI.
			unsigned uDeprecateAutoEOI : 1;
			//Bit 10 - 31 : Reserved
			unsigned : 22;
			// ebx
			unsigned uRecommendSpinLockRetry : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 32;
		};
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x40000004> const & cpuid)
		{
			return os
				// eax
				//Bit 0: Recommend using hypercall for address space switches rather than MOV to CR3 instruction
				<< (cpuid.uRecommendAddressSpaceSwitch ? " +RecommendAddressSpaceSwitch" : " -RecommendAddressSpaceSwitch")
				//Bit 1 : Recommend using hypercall for local TLB flushes rather than INVLPG or MOV to CR3 instructions
				<< (cpuid.uRecommendLocalTLB_Flushes ? " +RecommendLocalTLB_Flushes" : " -RecommendLocalTLB_Flushes")
				//Bit 2 : Recommend using hypercall for remote TLB flushes rather than inter - processor interrupts
				<< (cpuid.uRecommendRemoteTLB_Flushes ? " +RecommendRemoteTLB_Flushes" : " -RecommendRemoteTLB_Flushes")
				//Bit 3 : Recommend using MSRs for accessing APIC registers EOI, ICR and TPR rather than their memory - mapped counterparts.
				<< (cpuid.uRecommendMSR_ForAPIC_Registers ? " +RecommendMSR_ForAPIC_Registers" : " -RecommendMSR_ForAPIC_Registers")
				//Bit 4 : Recommend using the hypervisor - provided MSR to initiate a system RESET.
				<< (cpuid.uRecommendMSR_ToInitiateSystemRESET ? " +RecommendMSR_ToInitiateSystemRESET" : " -RecommendMSR_ToInitiateSystemRESET")
				//Bit 5 : Recommend using relaxed timing for this partition.If used, the VM should disable any watchdog timeouts that rely on the timely delivery of external interrupts.
				<< (cpuid.uRecommendRelaxedTiming ? " +RecommendRelaxedTiming" : " -RecommendRelaxedTiming")
				//Bit 6 : Recommend using DMA remapping.
				<< (cpuid.uUseDMA_Remapping ? " +UseDMA_Remapping" : " -UseDMA_Remapping")
				//Bit 7 : Recommend using interrupt remapping.
				<< (cpuid.uUseInterruptRemapping ? " +UseInterruptRemapping" : " -UseInterruptRemapping")
				//Bit 8 : Recommend using x2APIC MSRs.
				<< (cpuid.uUse_x2APIC_MSRs ? " +Use_x2APIC_MSRs" : " -Use_x2APIC_MSRs")
				//Bit 9 : Recommend deprecating AutoEOI.
				<< (cpuid.uDeprecateAutoEOI ? " +DeprecateAutoEOI" : " -DeprecateAutoEOI")
				//Bit 10 - 31 : Reserved
				// ebx
				<< " RecommendSpinLockRetry=" << cpuid.uRecommendSpinLockRetry
				// ecx
				// edx
				;
		}
		template <> struct cpuid_info_t<0x40000005>
		{
			//EAX
			//The maximum number of virtual processors supported.
			unsigned uMaxOfVirtualProcessors : 32;
			//EBX
			//The maximum number of logical processors supported.
			unsigned uMaxOfLogicalProcessors : 32;
			//ECX
			//The maximum number of physical interrupt vectors available for interrupt remapping.
			unsigned uMaxOfPhysicalInterruptVectors : 32;
			//EDX
			//Reserved
			unsigned : 32;
		};
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x40000005> const & cpuid)
		{
			return os
				//EAX
				//The maximum number of virtual processors supported.
				<< " MaxOfVirtualProcessors=" << cpuid.uMaxOfVirtualProcessors
				//EBX
				//The maximum number of logical processors supported.
				<< " MaxOfLogicalProcessors=" << cpuid.uMaxOfLogicalProcessors
				//ECX
				//The maximum number of physical interrupt vectors available for interrupt remapping.
				<< " MaxOfPhysicalInterruptVectors=" << cpuid.uMaxOfPhysicalInterruptVectors
				//EDX
				//Reserved
				;
		}
		template <> struct cpuid_info_t<0x40000006>
		{
			// eax
			//Bit 0: Support for APIC overlay assist is detected and in use.
			unsigned uAPIC_OverlayAssist : 1;
			//Bit 1 : Support for MSR bitmaps is detected and in use.
			unsigned uMSR_Bitmaps : 1;
			//Bit 2 : Support for architectural performance counters is detected and in use.
			unsigned uArchitecturalPerformanceCounters : 1;
			//Bit 3 : Support for second level address translation is detected and in use.
			unsigned uSecondLevelAddressTranslation : 1;
			//Bit 4 : Support for DMA remapping is detected and in use.
			unsigned uDMA_Remapping : 1;
			//Bit 5 : Support for interrupt remapping is detected and in use.
			unsigned uInterruptRemapping : 1;
			//Bit 6 : MemoryPatrolScrubberPresent
			unsigned uMemoryPatrolScrubberPresent : 1;
			//Bits 731 : Reserved for future Intel - specific features.
			unsigned : 25;
			// ebx
			unsigned : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 32;
		};
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x40000006> const & cpuid)
		{
			return os
				// eax
				//Bit 0: Support for APIC overlay assist is detected and in use.
				<< (cpuid.uAPIC_OverlayAssist ? " +APIC_OverlayAssist" : " -APIC_OverlayAssist")
				//Bit 1 : Support for MSR bitmaps is detected and in use.
				<< (cpuid.uMSR_Bitmaps ? " +MSR_Bitmaps" : " -MSR_Bitmaps")
				//Bit 2 : Support for architectural performance counters is detected and in use.
				<< (cpuid.uArchitecturalPerformanceCounters ? " +ArchitecturalPerformanceCounters" : " -ArchitecturalPerformanceCounters")
				//Bit 3 : Support for second level address translation is detected and in use.
				<< (cpuid.uSecondLevelAddressTranslation ? " +SecondLevelAddressTranslation" : " -SecondLevelAddressTranslation")
				//Bit 4 : Support for DMA remapping is detected and in use.
				<< (cpuid.uDMA_Remapping ? " +DMA_Remapping" : " -DMA_Remapping")
				//Bit 5 : Support for interrupt remapping is detected and in use.
				<< (cpuid.uInterruptRemapping ? " +InterruptRemapping" : " -InterruptRemapping")
				//Bit 6 : MemoryPatrolScrubberPresent
				<< (cpuid.uMemoryPatrolScrubberPresent ? " +MemoryPatrolScrubberPresent" : " -MemoryPatrolScrubberPresent")
				//Bits 731 : Reserved for future Intel - specific features.
				// ebx
				// ecx
				// edx
				;
		}
		template <> struct cpuid_info_t<0x80000000>
		{
			// eax
			unsigned uLFuncExt;
			// ebx
			char acVendor0[4];
			// ecx
			char acVendor2[4];
			// edx
			char acVendor1[4];

			unsigned max_leaf() const
			{
				return uLFuncExt;
			}
		};
		template <>
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000000> const & cpuid)
		{
			return (os << ' ').write(cpuid.acVendor0, 4).write(cpuid.acVendor1, 4).write(cpuid.acVendor2, 4);
		}
		template <> struct cpuid_info_t<0x80000001>
		{
			// eax
			unsigned uStepping : 4; // [bits 3:0]
			unsigned uBaseModel : 4; // [bits 7:4]
			unsigned uBaseFamily : 4; // [bits 11:8]
			unsigned : 4; // [bits 15:12]
			unsigned uExtModel : 4; // [bits 19:16]
			unsigned uExtFamily : 8; // [bits 27:20]
			unsigned : 4; // [bits 31:28]

			unsigned family() const
			{
				return uBaseFamily + uExtFamily;
			}
			unsigned model() const
			{
				return (uExtModel << 4) | uBaseModel;
			}

			//ebx
			unsigned uBrandID : 16; // [bits 15:0]
			unsigned : 12; // [bits 27:16]
			unsigned uPkgType : 4; // [bits 31:28]: Package type
								   // ecx
								   //0 LahfSahf LAHF and SAHF instruction support in 64 - bit mode.See “LAHF” and “SAHF” in APM3.
			unsigned uLahfSahf : 1;
			//1 CmpLegacy Core multi - processing legacy mode.See “Legacy Method” on page 624.
			unsigned uCmpLegacy : 1;
			//2 SVM Secure virtual machine.See “Secure Virtual Machine” in APM2.
			unsigned uSVM : 1;
			//3 ExtApicSpace Extended APIC space.This bit indicates the presence of extended APIC register space starting at offset 400h from the “APIC Base Address Register, ” as specified in the BKDG.
			unsigned uExtApicSpace : 1;
			//4 AltMovCr8 LOCK MOV CR0 means MOV CR8.See “MOV(CRn)” in APM3.
			unsigned uAltMovCr8 : 1;
			//5 ABM Advanced bit manipulation.LZCNT instruction support.See “LZCNT” in APM3.
			unsigned uABM : 1;
			//6 SSE4A EXTRQ, INSERTQ, MOVNTSS, and MOVNTSD instruction support.See “EXTRQ”, “INSERTQ”, “MOVNTSS”, and “MOVNTSD” in APM4.
			unsigned uSSE4A : 1;
			//7 MisAlignSse Misaligned SSE mode.See “Misaligned Access Support Added for SSE Instructions” in APM1.
			unsigned uMisAlignSse : 1;
			//8 3DNowPrefetch PREFETCH and PREFETCHW instruction support.See “PREFETCH” and “PREFETCHW” in APM3.
			unsigned u3DNowPrefetch : 1;
			//9 OSVW OS visible workaround.Indicates OS - visible workaround support.See “OS Visible Work - around(OSVW) Information” in APM2.
			unsigned uOSVW : 1;
			//10 IBS Instruction based sampling.See “Instruction Based Sampling” in APM2.
			unsigned uIBS : 1;
			//11 XOP Extended operation support.
			unsigned uXOP : 1;
			//12 SKINIT SKINIT and STGI are supported.Indicates support for SKINIT and STGI, independent of the value of MSRC000_0080[SVME].See APM2 and APM3.
			unsigned uSKINIT : 1;
			//13 WDT Watchdog timer support.See APM2 and APM3.Indicates support for MSRC001_0074.
			unsigned uWDT : 1;
			//14 — Reserved.
			unsigned : 1;
			//15 LWP Lightweight profiling support.See “Lightweight Profiling” in APM2 and reference pages for individual LWP instructions in APM3.
			unsigned uLWP : 1;
			//16 FMA4 Four - operand FMA instruction support.
			unsigned uFMA4 : 1;
			unsigned : 4;
			//21 TBM Trailing bit manipulation instruction support.
			unsigned uTBM : 1;
			//22 TopologyExtensions Topology extensions support.Indicates support for CPUID Fn8000_001D_EAX_x[N:0] - CPUID Fn8000_001E_EDX.
			unsigned uTopologyExtensions : 1;
			//23 PerfCtrExtCore Processor performance counter extensions support.Indicates support for MSRC001_020[A, 8, 6, 4, 2, 0] and MSRC001_020[B, 9, 7, 5, 3, 1].
			unsigned uPerfCtrExtCore : 1;
			//24 PerfCtrExtNB NB performance counter extensions support.Indicates support for MSRC001_024[6, 4, 2, 0] and MSRC001_024[7, 5, 3, 1].
			unsigned uPerfCtrExtNB : 1;
			//25 — Reserved
			unsigned : 1;
			//26 DataBreakpointEx tension Data access breakpoint extension.Indicates support for MSRC001_1027 and MSRC001_101[B:9].
			unsigned uDataBreakpointEx : 1;
			//27 PerfTsc Performance time - stamp counter.Indicates support for MSRC001_0280[Performance Time Stamp Counter].
			unsigned uPerfTsc : 1;
			unsigned : 1;
			unsigned uMONITORX : 1; // bit 29
			unsigned : 2;
			// edx
			//0 FPU x87 floating - point unit on - chip.Same as CPUID Fn0000_0001_EDX[FPU].
			unsigned uFPU : 1;
			//1 VME Virtual - mode enhancements.Same as CPUID Fn0000_0001_EDX[VME].
			unsigned uVME : 1;
			//2 DE Debugging extensions.Same as CPUID Fn0000_0001_EDX[DE].
			unsigned uDE : 1;
			//3 PSE Page - size extensions.Same as CPUID Fn0000_0001_EDX[PSE].
			unsigned uPSE : 1;
			//4 TSC Time stamp counter.Same as CPUID Fn0000_0001_EDX[TSC].
			unsigned uTSC : 1;
			//5 MSR AMD model - specific registers.Same as CPUID Fn0000_0001_EDX[MSR].
			unsigned uMSR : 1;
			//6 PAE Physical - address extensions.Same as CPUID Fn0000_0001_EDX[PAE].
			unsigned uPAE : 1;
			//7 MCE Machine check exception.Same as CPUID Fn0000_0001_EDX[MCE].
			unsigned uMCE : 1;
			//8 CMPXCHG8B CMPXCHG8B instruction.Same as CPUID Fn0000_0001_EDX[CMPXCHG8B].
			unsigned uCMPXCHG8B : 1;
			//9 APIC Advanced programmable interrupt controller.Same as CPUID //Fn0000_0001_EDX[APIC].
			unsigned uAPIC : 1;
			//10 — Reserved.
			unsigned : 1;
			//11 SysCallSysRet SYSCALL and SYSRET instructions. See “SYSCALL” and “SYSRET” in APM3. 
			unsigned uSysCallSysRet : 1;
			//12 MTRR Memory - type range registers.Same as CPUID Fn0000_0001_EDX[MTRR].
			unsigned uMTRR : 1;
			//13 PGE Page global extension.Same as CPUID Fn0000_0001_EDX[PGE].
			unsigned uPGE : 1;
			//14 MCA Machine check architecture.Same as CPUID Fn0000_0001_EDX[MCA].
			unsigned uMCA : 1;
			//15 CMOV Conditional move instructions.Same as CPUID Fn0000_0001_EDX[CMOV].
			unsigned uCMOV : 1;
			//16 PAT Page attribute table.Same as CPUID Fn0000_0001_EDX[PAT].
			unsigned uPAT : 1;
			//17 PSE36 Page - size extensions.Same as CPUID Fn0000_0001_EDX[PSE36].
			unsigned uPSE36 : 1;
			//19:18 — Reserved.
			unsigned : 2;
			//20 NX No-execute page protection. See “Page Translation and Protection” in APM2. 
			unsigned uNX : 1;
			unsigned : 1;
			//22 MmxExt AMD extensions to MMX instructions.See Appendix D “Instruction Subsets and CPUID Feature Sets” in APM3 and “128 - Bit Media and Scientific Programming” in APM1.
			unsigned uMmxExt : 1;
			unsigned : 2;
			//25 FFXSR FXSAVE and FXRSTOR instruction optimizations.See “FXSAVE” and “FXRSTOR” in APM5.
			unsigned uFFXSR : 1;
			//26 Page1GB 1-GB large page support. See “1-GB Paging Support” in APM2.
			unsigned uPage1GB : 1; // bit 26
								   //27 RDTSCP RDTSCP instruction. See “RDTSCP” in APM3.
			unsigned uRDTSCP : 1;
			unsigned : 1;
			//29 LM Long mode. See “Processor Initialization and Long-Mode Activation” in APM2. 
			unsigned uLM : 1;
			//30 3DNowExt AMD extensions to 3DNow!instructions.See Appendix D “Instruction Subsets and CPUID Feature Sets” in APM3.
			unsigned u3DNowExt : 1;
			//31 3DNow 3DNow!™ instructions.See Appendix D “Instruction Subsets and CPUID Feature Sets” in APM3.
			unsigned u3DNow : 1;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000001> const & cpuid)
		{
			return os
				//0 LahfSahf LAHF and SAHF instruction support in 64 - bit mode.See “LAHF” and “SAHF” in APM3.
				<< (cpuid.uLahfSahf ? " +LAHF +SAHF" : " -LAHF -SAHF")
				//1 CmpLegacy Core multi - processing legacy mode.See “Legacy Method” on page 624.
				<< (cpuid.uCmpLegacy ? " +CmpLegacy" : " -CmpLegacy")
				//2 SVM Secure virtual machine.See “Secure Virtual Machine” in APM2.
				<< (cpuid.uSVM ? " +SVM" : " -SVM")
				//3 ExtApicSpace Extended APIC space.This bit indicates the presence of extended APIC register space starting at offset 400h from the “APIC Base Address Register, ” as specified in the BKDG.
				<< (cpuid.uExtApicSpace ? " +ExtApicSpace" : " -ExtApicSpace")
				//4 AltMovCr8 LOCK MOV CR0 means MOV CR8.See “MOV(CRn)” in APM3.
				<< (cpuid.uAltMovCr8 ? " +AltMovCr8" : " -AltMovCr8")
				//5 ABM Advanced bit manipulation.LZCNT instruction support.See “LZCNT” in APM3.
				<< (cpuid.uABM ? " +ABM" : " -ABM")
				//6 SSE4A EXTRQ, INSERTQ, MOVNTSS, and MOVNTSD instruction support.See “EXTRQ”, “INSERTQ”, “MOVNTSS”, and “MOVNTSD” in APM4.
				<< (cpuid.uSSE4A ? " +SSE4A" : " -SSE4A")
				//7 MisAlignSse Misaligned SSE mode.See “Misaligned Access Support Added for SSE Instructions” in APM1.
				<< (cpuid.uMisAlignSse ? " +MisAlignSse" : " -MisAlignSse")
				//8 3DNowPrefetch PREFETCH and PREFETCHW instruction support.See “PREFETCH” and “PREFETCHW” in APM3.
				<< (cpuid.u3DNowPrefetch ? " +3DNowPrefetch" : " -3DNowPrefetch")
				//9 OSVW OS visible workaround.Indicates OS - visible workaround support.See “OS Visible Work - around(OSVW) Information” in APM2.
				<< (cpuid.uOSVW ? " +OSVW" : " -OSVW")
				//10 IBS Instruction based sampling.See “Instruction Based Sampling” in APM2.
				<< (cpuid.uIBS ? " +IBS" : " -IBS")
				//11 XOP Extended operation support.
				<< (cpuid.uXOP ? " +XOP" : " -XOP")
				//12 SKINIT SKINIT and STGI are supported.Indicates support for SKINIT and STGI, independent of the value of MSRC000_0080[SVME].See APM2 and APM3.
				<< (cpuid.uSKINIT ? " +SKINIT" : " -SKINIT")
				//13 WDT Watchdog timer support.See APM2 and APM3.Indicates support for MSRC001_0074.
				<< (cpuid.uWDT ? " +WDT" : " -WDT")
				//14 — Reserved.
				//15 LWP Lightweight profiling support.See “Lightweight Profiling” in APM2 and reference pages for individual LWP instructions in APM3.
				<< (cpuid.uLWP ? " +LWP" : " -LWP")
				//16 FMA4 Four - operand FMA instruction support.
				<< (cpuid.uFMA4 ? " +FMA4" : " -FMA4")
				//21 TBM Trailing bit manipulation instruction support.
				<< (cpuid.uTBM ? " +TBM" : " -TBM")
				//22 TopologyExtensions Topology extensions support.Indicates support for CPUID Fn8000_001D_EAX_x[N:0] - CPUID Fn8000_001E_EDX.
				<< (cpuid.uTopologyExtensions ? " +TopologyExtensions" : " -TopologyExtensions")
				//23 PerfCtrExtCore Processor performance counter extensions support.Indicates support for MSRC001_020[A, 8, 6, 4, 2, 0] and MSRC001_020[B, 9, 7, 5, 3, 1].
				<< (cpuid.uPerfCtrExtCore ? " +PerfCtrExtCore" : " -PerfCtrExtCore")
				//24 PerfCtrExtNB NB performance counter extensions support.Indicates support for MSRC001_024[6, 4, 2, 0] and MSRC001_024[7, 5, 3, 1].
				<< (cpuid.uPerfCtrExtNB ? " +PerfCtrExtNB" : " -PerfCtrExtNB")
				//25 — Reserved
				//26 DataBreakpointEx tension Data access breakpoint extension.Indicates support for MSRC001_1027 and MSRC001_101[B:9].
				<< (cpuid.uDataBreakpointEx ? " +DataBreakpointEx" : " -DataBreakpointEx")
				//27 PerfTsc Performance time - stamp counter.Indicates support for MSRC001_0280[Performance Time Stamp Counter].
				<< (cpuid.uPerfTsc ? " +PerfTsc" : " -PerfTsc")
				<< (cpuid.uMONITORX ? " +MONITORX" : " -MONITORX")
				<< (cpuid.uSysCallSysRet ? " +SYSCALL +SYSRET" : " -SYSCALL -SYSRET")
				<< (cpuid.uNX ? " +NX" : " -NX")
				<< (cpuid.uMmxExt ? " +MmxExt" : " -MmxExt")
				//25 FFXSR FXSAVE and FXRSTOR instruction optimizations.See “FXSAVE” and “FXRSTOR” in APM5.
				<< (cpuid.uFFXSR ? " +FFXSR" : " -FFXSR")
				<< (cpuid.uPage1GB ? " +Page1GB" : " -Page1GB")
				<< (cpuid.uRDTSCP ? " +RDTSCP" : " -RDTSCP")
				<< (cpuid.uLM ? " +LM" : " -LM")
				<< (cpuid.u3DNowExt ? " +3DNowExt" : " -3DNowExt")
				<< (cpuid.u3DNow ? " +3DNow!" : " -3DNow!");
		}
		template <> struct cpuid_info_t<0x80000002>
		{
			char ac[16];

			char const * processor_brand_string(void) const
			{
				return ac;
			}
		};
		template <> struct cpuid_info_t<0x80000003>
		{
			char ac[16];
		};
		template <> struct cpuid_info_t<0x80000004>
		{
			char ac[16];
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000002> const & cpuid) { return (os << ' ').write(cpuid.ac, 16); }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000003> const & cpuid) { return (os << ' ').write(cpuid.ac, 16); }
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000004> const & cpuid) { return (os << ' ').write(cpuid.ac, 16); }
		// Function 8000_0005h—L1 Cache and TLB Information
		template <> struct cpuid_info_t<0x80000005>
		{
			enum associativity : unsigned
			{
				ASSOC_RESERVED = 0,
				ASSOC_DIRECT_MAPPED = 1,
				ASSOC_FULLY_ASSOCIATIVE = 0xFFU,
			};

			// eax
			//7 : 0 L1ITlb2and4MSize Instruction TLB number of entries for 2 - MB and 4 - MB pages.The value returned is for the number of entries available for the 2 - MB page size; 4 - MB pages require two 2 - MB entries, so the number of entries available for the 4 - MB page size is one - half the returned value.
			unsigned uL1ITlb2and4MSize : 8;
			//15:8 L1ITlb2and4MAssoc Instruction TLB associativity for 2 - MB and 4 - MB pages.Encoding is per Table E - 3 below.
			associativity emL1ITlb2and4MAssoc : 8;
			//23 : 16 L1DTlb2and4MSize Data TLB number of entries for 2 - MB and 4 - MB pages.The value returned is for the number of entries available for the 2 - MB page size; 4 - MB pages require two 2 - MB entries, so the number of entries available for the 4 - MB page size is onehalf the returned value.
			unsigned uL1DTlb2and4MSize : 8;
			//31:24 L1DTlb2and4MAssoc Data TLB associativity for 2 - MB and 4 - MB pages.Encoding is per Table E - 3 below.
			associativity emL1DTlb2and4MAssoc : 8;
			// ebx
			//7 : 0 L1ITlb4KSize Instruction TLB number of entries for 4 KB pages.
			unsigned uL1ITlb4KSize : 8;
			//15 : 8 L1ITlb4KAssoc Instruction TLB associativity for 4 KB pages.Encoding is per Table E - 3 above.
			associativity emL1ITlb4KAssoc : 8;
			//23 : 16 L1DTlb4KSize Data TLB number of entries for 4 KB pages.
			unsigned uL1DTlb4KSize : 8;
			//31:24 L1DTlb4KAssoc Data TLB associativity for 4 KB pages.Encoding is per Table E - 3 above.
			associativity emL1DTlb4KAssoc : 8;
			// ecx
			//7 : 0 L1DcLineSize L1 data cache line size in bytes.
			unsigned uL1DcLineSize : 8;
			//15 : 8 L1DcLinesPerTag L1 data cache lines per tag.
			unsigned uL1DcLinesPerTag : 8;
			//23 : 16 L1DcAssoc L1 data cache associativity.Encoding is per Table E - 3.
			associativity emL1DcAssoc : 8;
			//31:24 L1DcSize L1 data cache size in KB.
			unsigned uL1DcSize : 8;
			// edx
			//7 : 0 L1IcLineSize L1 instruction cache line size in bytes.
			unsigned uL1IcLineSize : 8;
			//15 : 8 L1IcLinesPerTag L1 instruction cache lines per tag.
			unsigned uL1IcLinesPerTag : 8;
			//23 : 16 L1IcAssoc L1 instruction cache associativity.Encoding is per Table E - 3.
			associativity emL1IcAssoc : 8;
			//31:24 L1IcSize L1 instruction cache size in KB.
			unsigned uL1IcSize : 8;
		};
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000005>::associativity emAssociativity)
		{
			switch (emAssociativity)
			{
			case cpuid_info_t<0x80000005>::ASSOC_RESERVED:
				return os << "Reserved";
			case cpuid_info_t<0x80000005>::ASSOC_DIRECT_MAPPED:
				return os << "DirectMapped";
			case cpuid_info_t<0x80000005>::ASSOC_FULLY_ASSOCIATIVE:
				return os << "FullyAssociative";
			default:
				return os << static_cast<unsigned>(emAssociativity) << "way";
			}
		}
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000005> const & cpuid)
		{
			if (cpuid.emL1ITlb2and4MAssoc != cpuid.ASSOC_RESERVED)
			{
				os
					//7 : 0 L1ITlb2and4MSize Instruction TLB number of entries for 2 - MB and 4 - MB pages.The value returned is for the number of entries available for the 2 - MB page size; 4 - MB pages require two 2 - MB entries, so the number of entries available for the 4 - MB page size is one - half the returned value.
					<< " L1ITlb2and4MSize=" << cpuid.uL1ITlb2and4MSize
					//15:8 L1ITlb2and4MAssoc Instruction TLB associativity for 2 - MB and 4 - MB pages.Encoding is per Table E - 3 below.
					<< " L1ITlb2and4MAssoc=" << cpuid.emL1ITlb2and4MAssoc
					;
			}
			if (cpuid.emL1DTlb2and4MAssoc != cpuid.ASSOC_RESERVED)
			{
				os
					//23 : 16 L1DTlb2and4MSize Data TLB number of entries for 2 - MB and 4 - MB pages.The value returned is for the number of entries available for the 2 - MB page size; 4 - MB pages require two 2 - MB entries, so the number of entries available for the 4 - MB page size is onehalf the returned value.
					<< " L1DTlb2and4MSize=" << cpuid.uL1DTlb2and4MSize
					//31:24 L1DTlb2and4MAssoc Data TLB associativity for 2 - MB and 4 - MB pages.Encoding is per Table E - 3 below.
					<< " L1DTlb2and4MAssoc=" << cpuid.emL1DTlb2and4MAssoc
					;
			}
			if (cpuid.emL1ITlb4KAssoc != cpuid.ASSOC_RESERVED)
			{
				os
					//7 : 0 L1ITlb4KSize Instruction TLB number of entries for 4 KB pages.
					<< " L1ITlb4KSize=" << cpuid.uL1ITlb4KSize
					//15 : 8 L1ITlb4KAssoc Instruction TLB associativity for 4 KB pages.Encoding is per Table E - 3 above.
					<< " L1ITlb4KAssoc=" << cpuid.emL1ITlb4KAssoc
					;
			}
			if (cpuid.emL1DTlb4KAssoc != cpuid.ASSOC_RESERVED)
			{
				os
					//23 : 16 L1DTlb4KSize Data TLB number of entries for 4 KB pages.
					<< " L1DTlb4KSize=" << cpuid.uL1DTlb4KSize
					//31:24 L1DTlb4KAssoc Data TLB associativity for 4 KB pages.Encoding is per Table E - 3 above.
					<< " L1DTlb4KAssoc=" << cpuid.emL1DTlb4KAssoc
					;
			}
			if (cpuid.emL1DcAssoc != cpuid.ASSOC_RESERVED)
			{
				os
					//7 : 0 L1DcLineSize L1 data cache line size in bytes.
					<< " L1DcLineSize=" << cpuid.uL1DcLineSize << "B"
					//15 : 8 L1DcLinesPerTag L1 data cache lines per tag.
					<< " L1DcLinesPerTag=" << cpuid.uL1DcLinesPerTag
					//23 : 16 L1DcAssoc L1 data cache associativity.Encoding is per Table E - 3.
					<< " L1DcAssoc=" << cpuid.emL1DcAssoc
					//31:24 L1DcSize L1 data cache size in KB.
					<< " L1DcSize=" << cpuid.uL1DcSize << "KB"
					;
			}
			if (cpuid.emL1IcAssoc != cpuid.ASSOC_RESERVED)
			{
				os
					//7 : 0 L1IcLineSize L1 instruction cache line size in bytes.
					<< " L1IcLineSize=" << cpuid.uL1IcLineSize << "B"
					//15 : 8 L1IcLinesPerTag L1 instruction cache lines per tag.
					<< " L1IcLinesPerTag=" << cpuid.uL1IcLinesPerTag
					//23 : 16 L1IcAssoc L1 instruction cache associativity.Encoding is per Table E - 3.
					<< " L1IcAssoc=" << cpuid.emL1IcAssoc
					//31:24 L1IcSize L1 instruction cache size in KB.
					<< " L1IcSize=" << cpuid.uL1IcSize << "KB"
					;
			}
			return os;
		}
		// Function 8000_0006h—L2 Cache and TLB and L3 Cache Information
		template <> struct cpuid_info_t<0x80000006>
		{
			enum associativity : unsigned
			{
				ASSOC_DISABLED = 0U,
				ASSOC_DIRECT_MAPPED = 1U,
				ASSOC_2WAY = 2U,
				ASSOC_4WAY = 4U,
				ASSOC_8WAY = 6U,
				ASSOC_16WAY = 8U,
				ASSOC_32WAY = 0xAU,
				ASSOC_48WAY = 0xBU,
				ASSOC_64WAY = 0xCU,
				ASSOC_96WAY = 0xDU,
				ASSOC_128WAY = 0xEU,
				ASSOC_FULLY_ASSOCIATIVE = 0xFU,
			};

			// eax
			//11 : 0 L2ITlb2and4MSize L2 instruction TLB number of entries for 2 - MB and 4 - MB pages.The value returned is for the number of entries available for the 2 MB page size; 4 MB pages require two 2 MB entries, so the number of entries available for the 4 MB page size is one - half the returned value.
			unsigned uL2ITlb2and4MSize : 12;
			//15:12 L2ITlb2and4MAssoc L2 instruction TLB associativity for 2 - MB and 4 - MB pages.Encoding is per Table E - 4 below.
			associativity emL2ITlb2and4MAssoc : 4;
			//27 : 16 L2DTlb2and4MSize L2 data TLB number of entries for 2 - MB and 4 - MB pages.The value returned is for the number of entries available for the 2 MB page size; 4 MB pages require two 2 MB entries, so the number of entries available for the 4 MB page size is one - half the returned value.
			unsigned uL2DTlb2and4MSize : 12;
			//31:28 L2DTlb2and4MAssoc L2 data TLB associativity for 2 - MB and 4 - MB pages.Encoding is per Table E - 4 below.
			associativity emL2DTlb2and4MAssoc : 4;
			// ebx
			//11 : 0 L2ITlb4KSize L2 instruction TLB number of entries for 4 - KB pages.
			unsigned uL2ITlb4KSize : 12;
			//15 : 12 L2ITlb4KAssoc L2 instruction TLB associativity for 4 - KB pages.Encoding is per Table E - 4 above.
			associativity emL2ITlb4KAssoc : 4;
			//27 : 16 L2DTlb4KSize L2 data TLB number of entries for 4 - KB pages.
			unsigned uL2DTlb4KSize : 12;
			//31:28 L2DTlb4KAssoc L2 data TLB associativity for 4 - KB pages.Encoding is per Table E - 4 above.
			associativity emL2DTlb4KAssoc : 4;
			// ecx
			//7 : 0 L2LineSize L2 cache line size in bytes.
			unsigned uL2LineSize : 8;
			//11 : 8 L2LinesPerTag L2 cache lines per tag.
			unsigned uL2LinesPerTag : 4;
			//15 : 12 L2Assoc L2 cache associativity.Encoding is per Table E - 4 on page 611.
			associativity emL2Assoc : 4;
			//31:16 L2Size L2 cache size in KB.
			unsigned uL2Size : 16;
			// edx
			//7 : 0 L3LineSize L3 cache line size in bytes.
			unsigned uL3LineSize : 8;
			//11 : 8 L3LinesPerTag L3 cache lines per tag.
			unsigned uL3LinesPerTag : 4;
			//15 : 12 L3Assoc L3 cache associativity.Encoded per Table E - 4 on page 611.
			associativity emL3Assoc : 4;
			//17 : 16 — Reserved.
			unsigned : 2;
			//31:18 L3Size Specifies the L3 cache size range : (L3Size[31:18] * 512KB) ≤ L3 cache size < ((L3Size[31:18] + 1) * 512KB).
			unsigned uL3Size : 14;
		};
		std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000006>::associativity emAssociativity)
		{
			switch (emAssociativity)
			{
			case cpuid_info_t<0x80000006>::ASSOC_DISABLED:
				return os << "Disabled";
			case cpuid_info_t<0x80000006>::ASSOC_DIRECT_MAPPED:
				return os << "DirectMapped";
			case cpuid_info_t<0x80000006>::ASSOC_2WAY:
				return os << "2way";
			case cpuid_info_t<0x80000006>::ASSOC_4WAY:
				return os << "4way";
			case cpuid_info_t<0x80000006>::ASSOC_8WAY:
				return os << "8way";
			case cpuid_info_t<0x80000006>::ASSOC_16WAY:
				return os << "16way";
			case cpuid_info_t<0x80000006>::ASSOC_32WAY:
				return os << "32way";
			case cpuid_info_t<0x80000006>::ASSOC_48WAY:
				return os << "48way";
			case cpuid_info_t<0x80000006>::ASSOC_64WAY:
				return os << "64way";
			case cpuid_info_t<0x80000006>::ASSOC_96WAY:
				return os << "96way";
			case cpuid_info_t<0x80000006>::ASSOC_128WAY:
				return os << "128way";
			case cpuid_info_t<0x80000006>::ASSOC_FULLY_ASSOCIATIVE:
				return os << "FullyAssociative";
			default:
				return os;
			}
		}
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000006> const & cpuid)
		{
			if (cpuid.emL2ITlb2and4MAssoc != cpuid.ASSOC_DISABLED)
			{
				os
					//11 : 0 L2ITlb2and4MSize L2 instruction TLB number of entries for 2 - MB and 4 - MB pages.The value returned is for the number of entries available for the 2 MB page size; 4 MB pages require two 2 MB entries, so the number of entries available for the 4 MB page size is one - half the returned value.
					<< " L2ITlb2and4MSize=" << cpuid.uL2ITlb2and4MSize
					//15:12 L2ITlb2and4MAssoc L2 instruction TLB associativity for 2 - MB and 4 - MB pages.Encoding is per Table E - 4 below.
					<< " L2ITlb2and4MAssoc=" << cpuid.emL2ITlb2and4MAssoc
					;
			}
			if (cpuid.emL2DTlb2and4MAssoc != cpuid.ASSOC_DISABLED)
			{
				os
					//27 : 16 L2DTlb2and4MSize L2 data TLB number of entries for 2 - MB and 4 - MB pages.The value returned is for the number of entries available for the 2 MB page size; 4 MB pages require two 2 MB entries, so the number of entries available for the 4 MB page size is one - half the returned value.
					<< " L2DTlb2and4MSize=" << cpuid.uL2DTlb2and4MSize
					//31:28 L2DTlb2and4MAssoc L2 data TLB associativity for 2 - MB and 4 - MB pages.Encoding is per Table E - 4 below.
					<< " L2DTlb2and4MAssoc=" << cpuid.emL2DTlb2and4MAssoc
					;
			}
			if (cpuid.emL2ITlb4KAssoc != cpuid.ASSOC_DISABLED)
			{
				os
					//11 : 0 L2ITlb4KSize L2 instruction TLB number of entries for 4 - KB pages.
					<< " L2ITlb4KSize=" << cpuid.uL2ITlb4KSize
					//15 : 12 L2ITlb4KAssoc L2 instruction TLB associativity for 4 - KB pages.Encoding is per Table E - 4 above.
					<< " L2ITlb4KAssoc=" << cpuid.emL2ITlb4KAssoc
					;
			}
			if (cpuid.emL2DTlb4KAssoc != cpuid.ASSOC_DISABLED)
			{
				os
					//27 : 16 L2DTlb4KSize L2 data TLB number of entries for 4 - KB pages.
					<< " L2DTlb4KSize=" << cpuid.uL2DTlb4KSize
					//31:28 L2DTlb4KAssoc L2 data TLB associativity for 4 - KB pages.Encoding is per Table E - 4 above.
					<< " L2DTlb4KAssoc=" << cpuid.emL2DTlb4KAssoc
					;
			}
			if (cpuid.emL2Assoc != cpuid.ASSOC_DISABLED)
			{
				os
					<< " L2"
					//7 : 0 L2LineSize L2 cache line size in bytes.
					<< ':' << cpuid.uL2LineSize << 'B'
					//11 : 8 L2LinesPerTag L2 cache lines per tag.
					<< '*' << cpuid.uL2LinesPerTag
					//15 : 12 L2Assoc L2 cache associativity.Encoding is per Table E - 4 on page 611.
					<< '*' << cpuid.emL2Assoc
					//31:16 L2Size L2 cache size in KB.
					<< '=' << cpuid.uL2Size << "KB";
			}
			if (cpuid.emL3Assoc != cpuid.ASSOC_DISABLED)
			{
				os
					<< " L3"
					//7 : 0 L3LineSize L3 cache line size in bytes.
					<< ':' << cpuid.uL3LineSize << 'B'
					//11 : 8 L3LinesPerTag L3 cache lines per tag.
					<< '*' << cpuid.uL3LinesPerTag
					//15 : 12 L3Assoc L3 cache associativity.Encoded per Table E - 4 on page 611.
					<< '*' << cpuid.emL3Assoc
					//17 : 16 — Reserved.
					//31:18 L3Size Specifies the L3 cache size range : (L3Size[31:18] * 512KB) ≤ L3 cache size < ((L3Size[31:18] + 1) * 512KB).
					<< '=' << (cpuid.uL3Size * 512) << "KB";
			}
			return os;
		}
		// Function 8000_0007h—Processor Power Management and RAS Capabilities
		template <> struct cpuid_info_t<0x80000007>
		{
			// eax
			unsigned : 32;
			// ebx
			//0 McaOverflowRecov MCA overflow recovery support.If set, indicates that MCA overflow conditions(MCi_STATUS[Overflow] = 1) are not fatal; software may safely ignore such conditions.If clear, MCA overflow conditions require software to shut down the system.See APM2, Chapter 9, "Handling Machine Check Exceptions."
			unsigned uMcaOverflowRecov : 1;
			//1 SUCCOR Software uncorrectable error containment and recovery capability.The processor supports software containment of uncorrectable errors through context synchronizing data poisoning and deferred error interrupts; see APM2, Chapter 9, "Determining Machine - Check Architecture Support."
			unsigned uSUCCOR : 1;
			//2 HWA Hardware assert supported. Indicates support for MSRC001_10[DF:C0].
			unsigned uHWA : 1;
			unsigned : 29;
			// ecx
			//31:0 CpuPwrSampleTimeRatio Specifies the ratio of the compute unit power accumulator sample period to the TSC counter period.
			unsigned uCpuPwrSampleTimeRatio : 32;
			// edx
			//0 TS Temperature sensor.
			unsigned uTS : 1;
			//1 FID Frequency ID control.Function replaced by HwPstate.
			unsigned uFID : 1;
			//2 VID Voltage ID control.Function replaced by HwPstate.
			unsigned uVID : 1;
			//3 TTP THERMTRIP.
			unsigned uTTP : 1;
			//4 TM Hardware thermal control(HTC).
			unsigned uTM : 1;
			//5— Reserved.
			unsigned : 1;
			//6 100MHzSteps 100 MHz multiplier Control.
			unsigned u100MHzSteps : 1;
			//7 HwPstate Hardware P - state control.MSRC001_0061[P - state Current Limit], MSRC001_0062[P - state Control] and MSRC001_0063[P - state Status] exist.
			unsigned uHwPstate : 1;
			//8 TscInvariant TSC invariant.The TSC rate is ensured to be invariant across all P - States, CStates, and stop grant transitions(such as STPCLK Throttling); therefore the TSC is suitable for use as a source of time. 0 = No such guarantee is made and software should avoid attempting to use the TSC as a source of time.
			unsigned uTscInvariant : 1;
			//9 CPB Core performance boost.
			unsigned uCPB : 1;
			//10 EffFreqRO Read - only effective frequency interface. 1 = Indicates presence of MSRC000_00E7[Read - Only Max Performance Frequency Clock Count(MPerfReadOnly)] and MSRC000_00E8[Read - Only Actual Performance Frequency Clock Count(APerfReadOnly)].
			unsigned uEffFreqR0 : 1;
			//11 ProcFeedbackInterface Processor feedback interface.Value : 1. 1 = Indicates support for processor feedback interface.Note : This feature is deprecated.
			unsigned uProcFeedbackInterface : 1;
			//12 ProcPowerReporting Core power reporting interface supported.
			unsigned uProcPowerReporting : 1;
			unsigned : 19;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000007> const & cpuid)
		{
			return os
				//0 McaOverflowRecov MCA overflow recovery support.If set, indicates that MCA overflow conditions(MCi_STATUS[Overflow] = 1) are not fatal; software may safely ignore such conditions.If clear, MCA overflow conditions require software to shut down the system.See APM2, Chapter 9, "Handling Machine Check Exceptions."
				<< (cpuid.uMcaOverflowRecov ? " +McaOverflowRecov" : " -McaOverflowRecov")
				//1 SUCCOR Software uncorrectable error containment and recovery capability.The processor supports software containment of uncorrectable errors through context synchronizing data poisoning and deferred error interrupts; see APM2, Chapter 9, "Determining Machine - Check Architecture Support."
				<< (cpuid.uSUCCOR ? " +SUCCOR" : " -SUCCOR")
				//2 HWA Hardware assert supported. Indicates support for MSRC001_10[DF:C0].
				<< (cpuid.uHWA ? " +HWA" : " -HWA")
				//31:0 CpuPwrSampleTimeRatio Specifies the ratio of the compute unit power accumulator sample period to the TSC counter period.
				<< " CpuPwrSampleTimeRatio=" << cpuid.uCpuPwrSampleTimeRatio
				//0 TS Temperature sensor.
				<< (cpuid.uTS ? " +TS" : " -TS")
				//1 FID Frequency ID control.Function replaced by HwPstate.
				<< (cpuid.uFID ? " +FID" : " -FID")
				//2 VID Voltage ID control.Function replaced by HwPstate.
				<< (cpuid.uVID ? " +VID" : " -VID")
				//3 TTP THERMTRIP.
				<< (cpuid.uTTP ? " +TTP" : " -TTP")
				//4 TM Hardware thermal control(HTC).
				<< (cpuid.uTM ? " +TM" : " -TM")
				//5— Reserved.
				//6 100MHzSteps 100 MHz multiplier Control.
				<< (cpuid.u100MHzSteps ? " +100MHzSteps" : " -100MHzSteps")
				//7 HwPstate Hardware P - state control.MSRC001_0061[P - state Current Limit], MSRC001_0062[P - state Control] and MSRC001_0063[P - state Status] exist.
				<< (cpuid.uHwPstate ? " +HwPstate" : " -HwPstate")
				//8 TscInvariant TSC invariant.The TSC rate is ensured to be invariant across all P - States, CStates, and stop grant transitions(such as STPCLK Throttling); therefore the TSC is suitable for use as a source of time. 0 = No such guarantee is made and software should avoid attempting to use the TSC as a source of time.
				<< (cpuid.uTscInvariant ? " +TscInvariant" : " -TscInvariant")
				//9 CPB Core performance boost.
				<< (cpuid.uCPB ? " +CPB" : " -CPB")
				//10 EffFreqRO Read - only effective frequency interface. 1 = Indicates presence of MSRC000_00E7[Read - Only Max Performance Frequency Clock Count(MPerfReadOnly)] and MSRC000_00E8[Read - Only Actual Performance Frequency Clock Count(APerfReadOnly)].
				<< (cpuid.uEffFreqR0 ? " +EffFreqR0" : " -EffFreqR0")
				//11 ProcFeedbackInterface Processor feedback interface.Value : 1. 1 = Indicates support for processor feedback interface.Note : This feature is deprecated.
				<< (cpuid.uProcFeedbackInterface ? " +ProcFeedbackInterface" : " -ProcFeedbackInterface")
				//12 ProcPowerReporting Core power reporting interface supported.
				<< (cpuid.uProcPowerReporting ? " +ProcPowerReporting" : " -ProcPowerReporting")
				;
		}
		// Function 8000_0008h—Processor Capacity Parameters
		template <> struct cpuid_info_t<0x80000008>
		{
			// eax
			//7 : 0 PhysAddrSize Maximum physical byte address size in bits.When GuestPhysAddrSize is zero, this field also indicates the maximum guest physical address size.
			unsigned uPhysAddrSize : 8;
			//15 : 8 LinAddrSize Maximum linear byte address size in bits.
			unsigned uLinAddrSize : 8;
			//23:16 GuestPhysAddrSize Maximum guest physical byte address size in bits.This number applies only to guests using nested paging.When this field is zero, refer to the PhysAddrSize field for the maximum guest physical address size.See "Secure Virtual Machine" in APM2.
			unsigned uGuestPhysAddrSize : 8;
			unsigned : 8;
			// ebx
			unsigned : 32;
			// ecx
			//7:0 NC Number of physical cores - 1. The number of cores in the processor is NC + 1 (e.g., if NC = 0, then there is one core).See "Legacy Method" on page 624.
			unsigned uNC : 8;
			//11:8 — Reserved.
			unsigned : 4;
			//15 : 12 ApicIdCoreIdSize APIC ID size.The number of bits in the initial APIC20[ApicId] value that indicate core ID within a processor.A zero value indicates that legacy methods must be used to derive the maximum number of cores.The size of this field determines the maximum number of cores(MNC) that the processor could theoretically support, not the actual number of cores that are actually implemented or enabled on the processor, as indicated by CPUID Fn8000_0008_ECX[NC]. if (ApicIdCoreIdSize[3:0] == 0) { // Used by legacy dual-core/single-core processors MNC = CPUID Fn8000_0008_ECX[NC] + 1; } else { // use ApicIdCoreIdSize[3:0] field MNC = (2 ^ ApicIdCoreIdSize[3:0]); }
			unsigned uApicIdCoreIdSize : 4;
			//17:16 PerfTscSize Performance time - stamp counter size.Indicates the size of MSRC001_0280[PTSC].Bits Description 00b 40 bits 01b 48 bits 10b 56 bits 11b 64 bits
			enum performance_timestamp_counter_type : unsigned
			{
				PTSC_40BITS = 0,
				PTSC_48BITS = 1,
				PTSC_56BITS = 2,
				PTSC_64BITS = 3,
			} emPerfTscSize : 2;
			unsigned : 14;
			// edx
			unsigned : 32;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x80000008> const & cpuid)
		{
			return os
				//7 : 0 PhysAddrSize Maximum physical byte address size in bits.When GuestPhysAddrSize is zero, this field also indicates the maximum guest physical address size.
				<< " PhysAddrSize=" << cpuid.uPhysAddrSize
				//15 : 8 LinAddrSize Maximum linear byte address size in bits.
				<< " LinAddrSize=" << cpuid.uLinAddrSize
				//23:16 GuestPhysAddrSize Maximum guest physical byte address size in bits.This number applies only to guests using nested paging.When this field is zero, refer to the PhysAddrSize field for the maximum guest physical address size.See "Secure Virtual Machine" in APM2.
				<< " GuestPhysAddrSize=" << cpuid.uGuestPhysAddrSize
				//7:0 NC Number of physical cores - 1. The number of cores in the processor is NC + 1 (e.g., if NC = 0, then there is one core).See "Legacy Method" on page 624.
				<< " NC=" << cpuid.uNC
				//15 : 12 ApicIdCoreIdSize APIC ID size.The number of bits in the initial APIC20[ApicId] value that indicate core ID within a processor.A zero value indicates that legacy methods must be used to derive the maximum number of cores.The size of this field determines the maximum number of cores(MNC) that the processor could theoretically support, not the actual number of cores that are actually implemented or enabled on the processor, as indicated by CPUID Fn8000_0008_ECX[NC]. if (ApicIdCoreIdSize[3:0] == 0) { // Used by legacy dual-core/single-core processors MNC = CPUID Fn8000_0008_ECX[NC] + 1; } else { // use ApicIdCoreIdSize[3:0] field MNC = (2 ^ ApicIdCoreIdSize[3:0]); }
				<< " ApicIdCoreIdSize=" << cpuid.uApicIdCoreIdSize
				//17:16 PerfTscSize Performance time - stamp counter size.Indicates the size of MSRC001_0280[PTSC].Bits Description 00b 40 bits 01b 48 bits 10b 56 bits 11b 64 bits
				<< " PerfTscSize=" << cpuid.emPerfTscSize
				;
		}
		// Function 8000_000Ah—SVM Features
		template <> struct cpuid_info_t<0x8000000A>
		{
			// eax
			unsigned uSvmRev : 8;
			unsigned : 24;
			// ebx
			unsigned uNASID : 32;
			// ecx
			unsigned : 32;
			// edx
			//0 NP Nested paging.Indicates support for nested paging.See "Nested Paging."
			unsigned uNP : 1;
			//1 LbrVirt LBR virtualization.Indicates support for LBR Virtualization.See "Enabling LBR Virtualization."
			unsigned uLbrVirt : 1;
			//2 SVML SVM lock.Indicates support for SVM - Lock.See "Enabling SVM."
			unsigned uSVML : 1;
			//3 NRIPS NRIP save.Indicates support for NRIP save on #VMEXIT.See "State Saved on Exit."
			unsigned uNRIPS : 1;
			//4 TscRateMsr MSR based TSC rate control.Indicates support for MSR TSC ratio MSRC000_0104.See "TSC Ratio MSR(C000_0104h)."
			unsigned uTscRateMsr : 1;
			//5 VmcbClean VMCB clean bits.Indicates support for VMCB clean bits.See "VMCB Clean Bits."
			unsigned uVmcbClean : 1;
			//6 FlushByAsid  by ASID.Indicates that TLB flush events, including CR3 writes and CR4.PGE toggles, flush only the current ASID's TLB entries. Also indicates support for the extended VMCB TLB_Control. See "TLB Control." 
			unsigned uFlushByAsid : 1;
			//7 DecodeAssists Decode assists.Indicates support for the decode assists.See "Decode Assists."
			unsigned uDecodeAssists : 1;
			//9:8 — Reserved.
			unsigned : 2;
			//10 PauseFilter Pause intercept filter.Indicates support for the pause intercept filter.See "Pause Intercept Filtering."
			unsigned uPauseFilter : 1;
			//11 — Reserved.
			unsigned : 1;
			//12 PauseFilterThreshold PAUSE filter threshold.Indicates support for the PAUSE filter cycle count threshold.See "Pause Intercept Filtering."
			unsigned uPauseFilterThreshold : 1;
			//13 AVIC Support for the AMD advanced virtual interrupt controller.See "Advanced Virtual Interrupt Controller."
			unsigned uAVIC : 1;
			unsigned : 18;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x8000000A> const & cpuid)
		{
			return os
				//7:0 SvmRev SVM revision number.
				<< " SvmRev=" << cpuid.uSvmRev
				//31:0 NASID Number of available address space identifiers (ASID). 
				<< " NASID=" << cpuid.uNASID
				//0 NP Nested paging.Indicates support for nested paging.See "Nested Paging."
				<< (cpuid.uNP ? " +NP" : " -NP")
				//1 LbrVirt LBR virtualization.Indicates support for LBR Virtualization.See "Enabling LBR Virtualization."
				<< (cpuid.uLbrVirt ? " +LbrVirt" : " -LbrVirt")
				//2 SVML SVM lock.Indicates support for SVM - Lock.See "Enabling SVM."
				<< (cpuid.uSVML ? " +SVML" : " -SVML")
				//3 NRIPS NRIP save.Indicates support for NRIP save on #VMEXIT.See "State Saved on Exit."
				<< (cpuid.uNRIPS ? " +NRIPS" : " -NRIPS")
				//4 TscRateMsr MSR based TSC rate control.Indicates support for MSR TSC ratio MSRC000_0104.See "TSC Ratio MSR(C000_0104h)."
				<< (cpuid.uTscRateMsr ? " +TscRateMsr" : " -TscRateMsr")
				//5 VmcbClean VMCB clean bits.Indicates support for VMCB clean bits.See "VMCB Clean Bits."
				<< (cpuid.uVmcbClean ? " +VmcbClean" : " -VmcbClean")
				//6 FlushByAsid  by ASID.Indicates that TLB flush events, including CR3 writes and CR4.PGE toggles, flush only the current ASID's TLB entries. Also indicates support for the extended VMCB TLB_Control. See "TLB Control." 
				<< (cpuid.uFlushByAsid ? " +FlushByAsid" : " -FlushByAsid")
				//7 DecodeAssists Decode assists.Indicates support for the decode assists.See "Decode Assists."
				<< (cpuid.uDecodeAssists ? " +DecodeAssists" : " -DecodeAssists")
				//9:8 — Reserved.
				//10 PauseFilter Pause intercept filter.Indicates support for the pause intercept filter.See "Pause Intercept Filtering."
				<< (cpuid.uPauseFilter ? " +PauseFilter" : " -PauseFilter")
				//11 — Reserved.
				//12 PauseFilterThreshold PAUSE filter threshold.Indicates support for the PAUSE filter cycle count threshold.See "Pause Intercept Filtering."
				<< (cpuid.uPauseFilterThreshold ? " +PauseFilterThreshold" : " -PauseFilterThreshold")
				//13 AVIC Support for the AMD advanced virtual interrupt controller.See "Advanced Virtual Interrupt Controller."
				<< (cpuid.uAVIC ? " +AVIC" : " -AVIC")
				;
		};
		// Function 8000_001Bh—Instruction-Based Sampling Capabilities
		template <> struct cpuid_info_t<0x8000001B>
		{
			// eax
			//0 IBSFFV IBS feature flags valid. 
			unsigned uIBSFFV : 1;
			//1 FetchSam IBS fetch sampling supported.
			unsigned uFetchSam : 1;
			//2 OpSam IBS execution sampling supported.
			unsigned uOpSam : 1;
			//3 RdWrOpCnt Read write of op counter supported.
			unsigned uRdWrOpCnt : 1;
			//4 OpCnt Op counting mode supported.
			unsigned uOpCnt : 1;
			//5 BrnTrgt Branch target address reporting supported.
			unsigned uBrnTrgt : 1;
			//6 OpCntExt IbsOpCurCnt and IbsOpMaxCnt extend by 7 bits.
			unsigned uOpCntExt : 1;
			//7 RipInvalidChk Invalid RIP indication supported.
			unsigned uRipInvalidChk : 1;
			//8 OpBrnFuse Fused branch micro - op indication supported.
			unsigned uOpBrnFuse : 1;
			unsigned : 23;
			// ebx
			unsigned : 32;
			// ecx
			unsigned : 32;
			// edx
			unsigned : 32;
		};
		template <> std::ostream & operator << (std::ostream & os, cpuid_info_t<0x8000001B> const & cpuid)
		{
			return os
				//0 IBSFFV IBS feature flags valid. 
				<< (cpuid.uIBSFFV ? " +IBSFFV" : " -IBSFFV")
				//1 FetchSam IBS fetch sampling supported.
				<< (cpuid.uFetchSam ? " +FetchSam" : " -FetchSam")
				//2 OpSam IBS execution sampling supported.
				<< (cpuid.uOpSam ? " +OpSam" : " -OpSam")
				//3 RdWrOpCnt Read write of op counter supported.
				<< (cpuid.uRdWrOpCnt ? " +RdWrOpCnt" : " -RdWrOpCnt")
				//4 OpCnt Op counting mode supported.
				<< (cpuid.uOpCnt ? " +OpCnt" : " -OpCnt")
				//5 BrnTrgt Branch target address reporting supported.
				<< (cpuid.uBrnTrgt ? " +BrnTrgt" : " -BrnTrgt")
				//6 OpCntExt IbsOpCurCnt and IbsOpMaxCnt extend by 7 bits.
				<< (cpuid.uOpCntExt ? " +OpCntExt" : " -OpCntExt")
				//7 RipInvalidChk Invalid RIP indication supported.
				<< (cpuid.uRipInvalidChk ? " +RipInvalidChk" : " -RipInvalidChk")
				//8 OpBrnFuse Fused branch micro - op indication supported.
				<< (cpuid.uOpBrnFuse ? " +OpBrnFuse" : " -OpBrnFuse")
				;
		}

		template <unsigned uEAX, unsigned uECX = 0>
		struct cpuid_t
			: cpuid_info_t<uEAX, uECX>
		{
			static_assert(sizeof(cpuid_info_t<uEAX, uECX>) == 16, "CPUID expects 4 32-bit integers");

			explicit cpuid_t(unsigned uMaxEAX = static_cast<unsigned>(uEAX))
			{
				if (static_cast<unsigned>(uEAX) <= uMaxEAX)
				{
					__cpuidex(reinterpret_cast<int*>(this), uEAX, uECX);
				}
				else
				{
					memset(this, 0, sizeof(*this));
				}
			}
			cpuid_t(std::istream & is)
			{
				scan(is);
			}

			std::ostream & print(std::ostream & os) const
			{
				if (os.fail())
				{
					return os;
				}

				if (reinterpret_cast<int const*>(this)[0] != 0 ||
					reinterpret_cast<int const*>(this)[1] != 0 ||
					reinterpret_cast<int const*>(this)[2] != 0 ||
					reinterpret_cast<int const*>(this)[3] != 0)
				{
					int nMask = os.setf(std::ios_base::hex);
					os.unsetf(std::ios_base::dec);
					os << std::setw(8) << static_cast<unsigned>(uEAX)
						<< ' ' << std::setw(3) << static_cast<unsigned>(uECX)
						<< ' ' << std::setw(8) << reinterpret_cast<unsigned const*>(this)[0]
						<< ' ' << std::setw(8) << reinterpret_cast<unsigned const*>(this)[1]
						<< ' ' << std::setw(8) << reinterpret_cast<unsigned const*>(this)[2]
						<< ' ' << std::setw(8) << reinterpret_cast<unsigned const*>(this)[3];
					os.setf(nMask);
					os <<
						static_cast<cpuid_info_t<uEAX, uECX>const&>(*this) << std::endl;
				}
				return os;
			}
			std::istream & scan(std::istream & is)
			{
				reinterpret_cast<int*>(this)[0] = 0;
				reinterpret_cast<int*>(this)[1] = 0;
				reinterpret_cast<int*>(this)[2] = 0;
				reinterpret_cast<int*>(this)[3] = 0;

				if (is.fail())
				{
					return is;
				}

				std::streampos uPos = is.tellg();

				unsigned uLocalEAX = (~uEAX);
				unsigned uLocalECX = (~uECX);
				{
					int nMask = is.setf(std::ios_base::hex | std::ios_base::skipws);
					is.unsetf(std::ios_base::dec);
					is >>
						uLocalEAX >>
						uLocalECX >>
						reinterpret_cast<unsigned*>(this)[0] >>
						reinterpret_cast<unsigned*>(this)[1] >>
						reinterpret_cast<unsigned*>(this)[2] >>
						reinterpret_cast<unsigned*>(this)[3];
					is.setf(nMask);
				}

				if (is.fail() ||
					uLocalEAX != (uEAX) ||
					uLocalECX != (uECX))
				{
					is.clear();
					is.seekg(uPos);
					return is;
				}

				char acLocal[2000];
				do
				{
					is.getline(acLocal, sizeof(acLocal));
				} while (is.fail() && !is.eof());
				return is;
			}
		};
		template <unsigned uEAX, unsigned uECX>
		std::ostream & operator << (std::ostream & os, cpuid_t<uEAX, uECX> const & cpuid)
		{
			return cpuid.print(os);
		}
		template <unsigned uEAX, unsigned uECX>
		std::istream & operator >> (std::istream & is, cpuid_t<uEAX, uECX> & cpuid)
		{
			return cpuid.scan(is);
		}

		struct cpuid_soc_vendor_brand_string_t
			: cpuid_t<0x17, 1>
			, cpuid_t<0x17, 2>
			, cpuid_t<0x17, 3>
		{
			operator char const * (void) const
			{
				return reinterpret_cast<char const*>(this);
			}
		};
		std::ostream & operator << (std::ostream & os, cpuid_soc_vendor_brand_string_t const & cpuid)
		{
			return os.write(cpuid, sizeof(cpuid));
		}
		struct cpuid_processor_brand_string_t
			: cpuid_t<0x80000002>
			, cpuid_t<0x80000003>
			, cpuid_t<0x80000004>
		{
			operator char const * (void) const
			{
				return reinterpret_cast<char const*>(this);
			}
		};
		std::ostream & operator << (std::ostream & os, cpuid_processor_brand_string_t const & cpuid)
		{
			return os.write(cpuid, sizeof(cpuid));
		}

		//
		template <unsigned uEAX, unsigned uStart, unsigned uEnd, unsigned uSize = uEnd - uStart>
		struct cpuid_sub_leaf_t
			: cpuid_sub_leaf_t<uEAX, uStart, (uStart / 2 + uEnd / 2 + (uStart & uEnd & 1))>
			, cpuid_sub_leaf_t<uEAX, (uStart / 2 + uEnd / 2 + (uStart & uEnd & 1)), uEnd>
		{
			using left_t = cpuid_sub_leaf_t<uEAX, uStart, (uStart / 2 + uEnd / 2 + (uStart & uEnd & 1))>;
			using right_t = cpuid_sub_leaf_t<uEAX, (uStart / 2 + uEnd / 2 + (uStart & uEnd & 1)), uEnd>;

			explicit cpuid_sub_leaf_t(unsigned uMaxEAX = uEAX)
				: left_t(uMaxEAX)
				, right_t(uMaxEAX)
			{}
			explicit cpuid_sub_leaf_t(std::istream & is)
				: left_t(is)
				, right_t(is)
			{}
			std::ostream & print_sub_leaf(std::ostream & os, unsigned uMaxSubLeaf) const
			{
				left_t::print_sub_leaf(os, uMaxSubLeaf);
				right_t::print_sub_leaf(os, uMaxSubLeaf);
				return os;
			}
			std::istream & scan_sub_leaf(std::istream & is)
			{
				left_t::scan_sub_leaf(is);
				right_t::scan_sub_leaf(is);
				return is;
			}
		};
		template <unsigned uEAX, unsigned uECX1>
		struct cpuid_sub_leaf_t<uEAX, 0, uECX1, uECX1>
			: cpuid_t<uEAX>
			, cpuid_sub_leaf_t<uEAX, 1, uECX1, uECX1 - 1>
		{
			using base_t = cpuid_t<uEAX>;
			using next_t = cpuid_sub_leaf_t<uEAX, 1, uECX1, uECX1 - 1>;
			explicit cpuid_sub_leaf_t(unsigned uMaxEAX = uEAX)
				: base_t(uMaxEAX)
				, next_t(uMaxEAX)
			{}
			explicit cpuid_sub_leaf_t(std::istream & is)
				: base_t(is)
				, next_t(is)
			{}
			std::ostream & print_sub_leaf(std::ostream & os) const
			{
				base_t::print(os);
				next_t::print_sub_leaf(os, base_t::max_sub_leaf());
				return os;
			}
			std::istream & scan_sub_leaf(std::istream & is)
			{
				base_t::scan(is);
				next_t::scan_sub_leaf(is);
				return is;
			}
		};
		template <unsigned uEAX, unsigned uStart, unsigned uEnd>
		struct cpuid_sub_leaf_t<uEAX, uStart, uEnd, 1>
			: cpuid_t<uEAX, uStart>
		{
			using base_t = cpuid_t<uEAX, uStart>;
			explicit cpuid_sub_leaf_t(unsigned uMaxEAX = uEAX)
				: base_t(uMaxEAX)
			{}
			explicit cpuid_sub_leaf_t(std::istream & is)
				: base_t(is)
			{}
			std::ostream & print_sub_leaf(std::ostream & os, unsigned uMaxSubLeaf) const
			{
				if (base_t::is_sub_leaf(uMaxSubLeaf))
				{
					base_t::print(os);
				}
				return os;
			}
			std::istream & scan_sub_leaf(std::istream & is)
			{
				base_t::scan(is);
				return is;
			}
		};
		template <unsigned uEAX>
		struct cpuid_sub_leaf_t<uEAX, 0, 1, 1>
			: cpuid_t<uEAX>
		{
			using base_t = cpuid_t<uEAX>;
			explicit cpuid_sub_leaf_t(unsigned uMaxEAX = uEAX)
				: base_t(uMaxEAX)
			{}
			explicit cpuid_sub_leaf_t(std::istream & is)
				: base_t(is)
			{}
			std::ostream & print_sub_leaf(std::ostream & os) const
			{
				base_t::print(os);
				return os;
			}
			std::istream & scan_sub_leaf(std::istream & is)
			{
				base_t::scan(is);
				return is;
			}
		};
		template <unsigned uEAX> struct cpuid_leaf_traits
		{
			enum config { MAX_ECX = 0 };
		};
		template <> struct cpuid_leaf_traits<0x04> { enum config { MAX_ECX = 4 }; };
		template <> struct cpuid_leaf_traits<0x07> { enum config { MAX_ECX = 1 }; };
		template <> struct cpuid_leaf_traits<0x0B> { enum config { MAX_ECX = 2 }; };
		template <> struct cpuid_leaf_traits<0x0D> { enum config { MAX_ECX = 9 }; };
		template <> struct cpuid_leaf_traits<0x0F> { enum config { MAX_ECX = 2 }; };
		template <> struct cpuid_leaf_traits<0x10> { enum config { MAX_ECX = 1 }; };
		template <> struct cpuid_leaf_traits<0x14> { enum config { MAX_ECX = 1 }; };
		template <> struct cpuid_leaf_traits<0x17> { enum config { MAX_ECX = 3 }; };
		template <unsigned uEAX>
		struct cpuid_leaf_t
			: cpuid_sub_leaf_t<uEAX, 0, cpuid_leaf_traits<uEAX>::MAX_ECX + 1>
		{
			using base_t = cpuid_sub_leaf_t<uEAX, 0, cpuid_leaf_traits<uEAX>::MAX_ECX + 1>;
			explicit cpuid_leaf_t(unsigned uMaxEAX = static_cast<unsigned>(uEAX))
				: base_t(uMaxEAX)
			{}
			cpuid_leaf_t(std::istream & is)
				: base_t(is)
			{}
			std::ostream & print_leaf(std::ostream & os) const
			{
				base_t::print_sub_leaf(os);
				return os;
			}
			std::istream & scan_leaf(std::istream & is)
			{
				base_t::scan_sub_leaf(is);
				return is;
			}
		};
		template <unsigned uEAX>
		std::ostream & operator << (std::ostream & os, cpuid_leaf_t<uEAX> const & cpuid_leaf)
		{
			return cpuid_leaf.print_leaf(os);
		}
		template <unsigned uEAX>
		std::istream & operator >> (std::istream & is, cpuid_leaf_t<uEAX> & cpuid_leaf)
		{
			return cpuid_leaf.scan_leaf(is);
		}

		//

		template <unsigned uStart, unsigned uEnd, unsigned uSize = uEnd - uStart, unsigned uRoot = uStart & 0x3FFFFFFFU>
		struct cpuid_tree_t
			: cpuid_tree_t<uStart, (uStart / 2 + uEnd / 2 + (uStart & uEnd & 1))>
			, cpuid_tree_t<(uStart / 2 + uEnd / 2 + (uStart & uEnd & 1)), uEnd>
		{
			using left_t = cpuid_tree_t<uStart, (uStart / 2 + uEnd / 2 + (uStart & uEnd & 1))>;
			using right_t = cpuid_tree_t<(uStart / 2 + uEnd / 2 + (uStart & uEnd & 1)), uEnd>;
			explicit cpuid_tree_t(unsigned uMaxLeaf)
				: left_t(uMaxLeaf)
				, right_t(uMaxLeaf)
			{}
			explicit cpuid_tree_t(std::istream & is)
				: left_t(is)
				, right_t(is)
			{}
			std::ostream & print_tree(std::ostream & os, unsigned uMaxLeaf) const
			{
				left_t::print_tree(os, uMaxLeaf);
				right_t::print_tree(os, uMaxLeaf);
				return os;
			}
			std::istream & scan_tree(std::istream & is)
			{
				left_t::scan_tree(is);
				right_t::scan_tree(is);
				return is;
			}
		};
		template <unsigned uEAX, unsigned uEAX1, unsigned uRoot>
		struct cpuid_tree_t<uEAX, uEAX1, 1, uRoot>
			: cpuid_leaf_t<uEAX>
		{
			explicit cpuid_tree_t(unsigned uMaxLeaf)
				: cpuid_leaf_t<uEAX>(uMaxLeaf)
			{}
			explicit cpuid_tree_t(std::istream & is)
				: cpuid_leaf_t<uEAX>(is)
			{}
			std::ostream & print_tree(std::ostream & os, unsigned uMaxLeaf) const
			{
				if (static_cast<unsigned>(uEAX) <= uMaxLeaf)
				{
					cpuid_leaf_t<uEAX>::print_leaf(os);
				}
				return os;
			}
			std::istream & scan_tree(std::istream & is)
			{
				cpuid_leaf_t<uEAX>::scan_leaf(is);
				return is;
			}
		};
		template <unsigned uStart, unsigned uEnd, unsigned uSize>
		struct cpuid_tree_t<uStart, uEnd, uSize, 0>
			: cpuid_leaf_t<uStart>
			, cpuid_tree_t<uStart + 1, uEnd>
		{
			using leaf_t = cpuid_leaf_t<uStart>;
			using tree_t = cpuid_tree_t<uStart + 1, uEnd>;
			cpuid_tree_t()
				: tree_t(max_leaf())
			{}
			cpuid_tree_t(std::istream & is)
				: leaf_t(is)
				, tree_t(is)
			{}
			std::ostream & print_tree(std::ostream & os) const
			{
				leaf_t::print_leaf(os);
				tree_t::print_tree(os, max_leaf());
				return os;
			}
			std::istream & scan_tree(std::istream & is)
			{
				leaf_t::scan_leaf(is);
				tree_t::scan_tree(is);
				return is;
			}
		};
		template <unsigned uStart, unsigned uEnd>
		std::ostream & operator << (std::ostream & os, cpuid_tree_t<uStart, uEnd> const & cpuid)
		{
			return cpuid.print_tree(os);
		}
		template <unsigned uStart, unsigned uEnd>
		std::istream & operator >> (std::istream & is, cpuid_tree_t<uStart, uEnd> & cpuid)
		{
			return cpuid.scan_tree(is);
		}

		using cpuid_standard_tree_t = cpuid_tree_t<0x00, 0x17>;
		using cpuid_hypervisor_tree_t = cpuid_tree_t<0x40000000, 0x400000FF>;
		using cpuid_extended_tree_t = cpuid_tree_t<0x80000000U, 0x8000001BU>;

		struct cpuid_forest_t
			: cpuid_standard_tree_t
			, cpuid_hypervisor_tree_t
			, cpuid_extended_tree_t
		{
			cpuid_forest_t()
			{}
			cpuid_forest_t(std::istream & is)
				: cpuid_standard_tree_t(is)
				, cpuid_hypervisor_tree_t(is)
				, cpuid_extended_tree_t(is)
			{}
			std::ostream & print_forest(std::ostream & os) const
			{
				cpuid_standard_tree_t::print_tree(os);
				if (uHypervisorPresent)
					cpuid_hypervisor_tree_t::print_tree(os);
				cpuid_extended_tree_t::print_tree(os);
				return os;
			}
			std::istream & scan_forest(std::istream & is)
			{
				cpuid_standard_tree_t::scan_tree(is);
				if (uHypervisorPresent)
					cpuid_hypervisor_tree_t::scan_tree(is);
				cpuid_extended_tree_t::scan_tree(is);
				return is;
			}
		};
		std::ostream & operator << (std::ostream & os, cpuid_forest_t const & cpuid)
		{
			return cpuid.print_forest(os);
		}
		std::istream & operator >> (std::istream & is, cpuid_forest_t & cpuid)
		{
			return cpuid.scan_forest(is);
		}
	}
}
