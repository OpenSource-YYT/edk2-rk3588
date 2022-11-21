/** @file
  ResetSystemLib implementation for RK35xx platform

  Copyright (c) 2017 - 2018, Linaro Ltd. All rights reserved.
  Copyright (c) 2019, Intel Corporation. All rights reserved.
  Copyright (c) 2022, Xilin Wu <wuxilin123@gmail.com>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <Library/ArmSmcLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/ResetSystemLib.h>

#include <IndustryStandard/ArmStdSmc.h>

/**
  This function causes a system-wide initialization (warm reset), in which all processors
  are set to their initial state. Pending cycles are not corrupted.

  If this function returns, it means that the system does not support warm reset.
**/
VOID
EFIAPI
ResetWarm (
  VOID
  )
{
  // First global software reset by programming CRU_GLB_SRST_FST as 0xfdb9
#ifdef RK356X
  MmioWrite32 (0xFDD200D4, 0xfdb9);
#else
  MmioWrite32 (0xFD7C0C08, 0xfdb9);
#endif
}

/**
  This function causes a system-wide reset (cold reset), in which
  all circuitry within the system returns to its initial state. This type of reset
  is asynchronous to system operation and operates without regard to
  cycle boundaries.

  If this function returns, it means that the system does not support cold reset.
**/
VOID
EFIAPI
ResetCold (
  VOID
  )
{
  ResetWarm();
  // Doesn't work somehow
  // ARM_SMC_ARGS ArmSmcArgs;

  // // Send a PSCI 0.2 SYSTEM_RESET command
  // ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_SYSTEM_RESET;
  // ArmCallSmc (&ArmSmcArgs);
}

/**
  This function causes the system to enter a power state equivalent
  to the ACPI G2/S5 or G3 states.

  If this function returns, it means that the system does not support shutdown reset.
**/
VOID
EFIAPI
ResetShutdown (
  VOID
  )
{
  ARM_SMC_ARGS ArmSmcArgs;

  // Send a PSCI 0.2 SYSTEM_OFF command
  ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_SYSTEM_OFF;
  ArmCallSmc (&ArmSmcArgs);
}

/**
  This function causes a systemwide reset. The exact type of the reset is
  defined by the EFI_GUID that follows the Null-terminated Unicode string passed
  into ResetData. If the platform does not recognize the EFI_GUID in ResetData
  the platform must pick a supported reset type to perform.The platform may
  optionally log the parameters from any non-normal reset that occurs.

  @param[in]  DataSize   The size, in bytes, of ResetData.
  @param[in]  ResetData  The data buffer starts with a Null-terminated string,
                         followed by the EFI_GUID.
**/
VOID
EFIAPI
ResetPlatformSpecific (
  IN UINTN  DataSize,
  IN VOID   *ResetData
  )
{
#ifdef RK356X
  MmioWrite32 (0xfdc20200, 0xEF08A53C);
#else
  MmioWrite32 (0xfd588080, 0xEF08A53C);
#endif
  // Hard reset doesn't work somehow
  ResetWarm();
}

/**
  The ResetSystem function resets the entire platform.

  @param[in] ResetType      The type of reset to perform.
  @param[in] ResetStatus    The status code for the reset.
  @param[in] DataSize       The size, in bytes, of ResetData.
  @param[in] ResetData      For a ResetType of EfiResetCold, EfiResetWarm, or EfiResetShutdown
                            the data buffer starts with a Null-terminated string, optionally
                            followed by additional binary data. The string is a description
                            that the caller may use to further indicate the reason for the
                            system reset.
**/
VOID
EFIAPI
ResetSystem (
  IN EFI_RESET_TYPE  ResetType,
  IN EFI_STATUS      ResetStatus,
  IN UINTN           DataSize,
  IN VOID            *ResetData OPTIONAL
  )
{
  switch (ResetType) {
    case EfiResetWarm:
      ResetWarm ();
      break;

    case EfiResetCold:
      ResetCold ();
      break;

    case EfiResetShutdown:
      ResetShutdown ();
      return;

    case EfiResetPlatformSpecific:
      ResetPlatformSpecific (DataSize, ResetData);
      return;

    default:
      return;
  }
}
