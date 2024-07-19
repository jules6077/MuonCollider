# Setup up the Envireonment for Realistic Digi Processor
### Apptainer: 
{code} apptainer shell docker://gitlab-registry.cern.ch/muon-collider/mucoll-deploy/mucoll:2.8-patch2-el9 {code}
### environment:
source /opt/setup_mucoll.sh
### setup up that environment from TrkHitsStudiesWorkspace:
source path/to/TrkHitsStudiesWorkspace/setup.sh path/to/TrkHitsStudiesWorkspace/build

# Now I can run my code: 
### First I start by running my shell code:
cd MCJ_configs/testing/runConfig.sh

### The Problem is that the resulting .slcio file still has zero IT and OT Barrel hits. 
### You Can see the code that I am running in MCJ_configs/config.xml, which is form the MC_configs github which I modified
