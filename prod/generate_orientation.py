from __future__ import print_function

import os

nevents = 2000
nfiles  = 10
orientation = "parallel" # to EL plane
#orientation = "perpendicular" # to EL plane

x = 0
y = 0
z = 250

px = 0 if orientation == "perpendicular" else 1
py = 0
pz = 1 if orientation == "perpendicular" else 0

prod_path = "/home/gonzalo/sw/git/nexus/prod/"
macs_path = "/home/gonzalo/data/NEXT/zlength/track_orientation/{orientation}/macs/".format(**locals())
data_path = "/home/gonzalo/data/NEXT/zlength/track_orientation/{orientation}/data/".format(**locals())
jobs_path = "/home/gonzalo/data/NEXT/zlength/track_orientation/{orientation}/jobs/".format(**locals())
logs_path = "/home/gonzalo/data/NEXT/zlength/track_orientation/{orientation}/logs/".format(**locals())

for _, path in filter(lambda (x, _): "_path" in x, locals().items()):
    if not os.path.exists(path):
        print("Creating", path)
        os.makedirs(path)

init_filename = "NEW_track.init.template"
conf_filename = "NEW_track.config.template".format(**locals())
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
    datafile =               "nexus_{orientation}_{index}.next".format(**locals())
    start_id = nevt_per_file * index
    n_evt    = nevt_per_file

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
