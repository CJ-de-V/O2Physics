o2physics_add_dpl_workflow(muon-reader
                    SOURCES muonReader.cxx
                    PUBLIC_LINK_LIBRARIES O2::Framework O2Physics::AnalysisCore O2Physics::PWGDQCore
                    COMPONENT_NAME Analysis)
