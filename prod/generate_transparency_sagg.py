from __future__ import print_function

import os
import sys

nevents  = 20
nfiles   = 1
sagging  = sys.argv[1]
region   = sys.argv[2]
x        = dict(center=0, mid=90, border=180)[region]
y        =  0
z        = 10

mesh_sagging = sagging

prod_path = "/home/gonzalo/sw/git/nexus/prod/"
macs_path = "/home/gonzalo/data/NEXT/ELsim/transparency/{sagging}mm/{region}/macs/".format(**locals())
data_path = "/home/gonzalo/data/NEXT/ELsim/transparency/{sagging}mm/{region}/data/".format(**locals())
jobs_path = "/home/gonzalo/data/NEXT/ELsim/transparency/{sagging}mm/{region}/jobs/".format(**locals())
logs_path = "/home/gonzalo/data/NEXT/ELsim/transparency/{sagging}mm/{region}/logs/".format(**locals())

for _, path in filter(lambda (x, _): "_path" in x, locals().items()):
    if not os.path.exists(path):
        print("Creating", path)
        os.makedirs(path)

init_filename = "new_mesh_transparency_sagg.init.template"
conf_filename = "new_mesh_transparency_sagg.config.template"
job_filename  = "launch_nexus.template"

init_template = open(os.path.join(prod_path, init_filename)).read()
conf_template = open(os.path.join(prod_path, conf_filename)).read()
job_template  = open(os.path.join(prod_path,  job_filename)).read()

print("Writing files")

nevt_per_file = nevents // nfiles
for index in range(nfiles):
    initfile = init_filename.replace("template", "{index}.mac" .format(**locals()))
    conffile = conf_filename.replace("template", "{index}.mac" .format(**locals()))
    jobfile  =  job_filename.replace("template", "{index}.sh"  .format(**locals()))
    logfile  =                                   "{index}.txt" .format(**locals())
    datafile =                                   "{index}.next".format(**locals())
    n_evt    = nevt_per_file
    start_id = nevt_per_file * index

    initfile   = os.path.join(macs_path, initfile)
    conffile   = os.path.join(macs_path, conffile)
    jobfile    = os.path.join(jobs_path,  jobfile)
    logfile    = os.path.join(logs_path,  logfile)
    outputfile = os.path.join(data_path, datafile)

    with open(initfile, "w") as file:
        file.write(init_template.format(**locals()))

    with open(conffile, "w") as file:
        file.write(conf_template.format(**locals()))

    with open(jobfile, "w") as file:
        file.write(job_template.format(**locals()))
