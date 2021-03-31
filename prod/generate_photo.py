from __future__ import print_function

import os

nevents = 10
nfiles  = 1
region  = "center"

prod_path = "/home/gonzalo/sw/git/nexus/prod/"
macs_path = "/home/gonzalo/data/NEXT/ELsim/photo/{region}/macs/".format(**locals())
data_path = "/home/gonzalo/data/NEXT/ELsim/photo/{region}/data/".format(**locals())
jobs_path = "/home/gonzalo/data/NEXT/ELsim/photo/{region}/jobs/".format(**locals())
logs_path = "/home/gonzalo/data/NEXT/ELsim/photo/{region}/logs/".format(**locals())

for _, path in filter(lambda (x, _): "_path" in x, locals().items()):
    if not os.path.exists(path):
        print("Creating", path)
        os.makedirs(path)

init_filename = "new_photon_tracking.init.template"
conf_filename = "new_photon_tracking.config.{region}.template".format(**locals())
job_filename  = "launch_nexus.template"

init_template = open(os.path.join(prod_path, init_filename)).read()
conf_template = open(os.path.join(prod_path, conf_filename)).read()
job_template  = open(os.path.join(prod_path,  job_filename)).read()

print("Writing files")

nevt_per_file = nevents // nfiles
for index in range(nfiles):
    initfile = init_filename.replace("template", "{region}.{index}.mac".format(**locals()))
    conffile = conf_filename.replace("template",          "{index}.mac".format(**locals()))
    jobfile  =  job_filename.replace("template", "{region}.{index}.sh" .format(**locals()))
    logfile  =                                  "{region}_{index}.txt" .format(**locals())
    datafile =                                  "{region}_{index}.next".format(**locals())
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
