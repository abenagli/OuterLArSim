#!/usr/bin/env python3

import json

tasks = {}

for WLSConc in range(20):
    tasks['vs_WLSConc%d'%WLSConc] = {}
    tasks['vs_WLSConc%d'%WLSConc]['cfg'] = 'config_slats.cfg'
    tasks['vs_WLSConc%d'%WLSConc]['outfile'] = 'out.root'
    tasks['vs_WLSConc%d'%WLSConc]['WLSConc'] = '%d'%WLSConc
    tasks['vs_WLSConc%d'%WLSConc]['det_conf'] = 'longUD'
    tasks['vs_WLSConc%d'%WLSConc]['det_N'] = '16'

    with open('tasks.json', "w") as file:
        json.dump(tasks, file, indent=4)
