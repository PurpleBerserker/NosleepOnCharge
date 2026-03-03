/*
	Copyright (C) 2017, PurpleBerserker

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <psp2kern/kernel/suspend.h>
#include <psp2kern/kernel/threadmgr.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/debug.h>
#include <psp2kern/power.h>
#include "logging/log.h"

// SCE_KERNEL_POWER_TICK_DEFAULT - Cancel all timers
// SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND - Cancel automatic suspension timer
// SCE_KERNEL_POWER_TICK_DISABLE_OLED_OFF - Cancel OLED-off timer
// SCE_KERNEL_POWER_TICK_DISABLE_OLED_DIMMING -  Cancel OLED dimming timer


/*
	TODO: register a suspend callback that calls the sceKernelPowerTick function
	Suggestion by @xerpi
*/

int nosleep_thread(SceSize args, void *argp) {
    while (1) {
        // Vérifie charge (KERNEL API)
        int is_charging = kscePowerIsPowerOnline();

        if (is_charging == SCE_TRUE) {
            // Désactive veille automatique et l'arret de l'ecran OLED
            ksceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND );
            ksceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_OLED_OFF );
            LOG("NoSleepOnCharge: Ticks reinitialized\n");
        }

        ksceKernelDelayThread(1000000); // 1 seconde
    }
    return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));

// Point d'entrée KERNEL plugin
int module_start(SceSize argc, const void *args) {
    // Crée thread surveillance
    SceUID thid = ksceKernelCreateThread("NoSleep", nosleep_thread, 0x40, 0x10000, 0, 0, NULL);

    if (thid >= 0) {
        ksceKernelStartThread(thid, 0, NULL);
        LOG("NoSleepOnCharge: Thread %08X lancé\n", thid);
    } else {
        LOG("NoSleepOnCharge: ERREUR thread %08X\n", thid);
    }

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
    LOG("NoSleepOnCharge: Module arrêté\n");
    return SCE_KERNEL_STOP_SUCCESS;
}
