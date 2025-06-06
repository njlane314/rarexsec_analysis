#include <iostream>
#include "framework/AnalysisFramework.h"
#include "framework/CommonAnalysisSpace.h"
#include "TFile.h"

int main() {
    try {
        ROOT::EnableImplicitMT();

        AnalysisFramework::DataManager data_manager({
            .config_file = "../config.json",
            .beam_key = "numi_fhc",
            .runs_to_load = {"run1"},
            .blinded = true,
            .variable_options = {
                .load_reco_event_info = true, .load_reco_track_info = true,
                .load_truth_event_info = true, .load_weights_and_systematics = true,
                .load_reco_shower_info = true, .load_blip_info = true
            }
        });

        AnalysisFramework::AnalysisSpace analysis_space = AnalysisFramework::GetCommonAnalysisSpace();
        AnalysisFramework::SystematicsController systematics_controller(data_manager.getVariableManager());
        AnalysisFramework::AnalysisRunner runner({
            .data_manager = data_manager,
            .analysis_space = analysis_space,
            .systematics_controller = systematics_controller
        });

        auto results = runner.run();
        std::cout << "Analysis run completed successfully." << std::endl;

        TFile* outFile = TFile::Open("analysis_results.root", "RECREATE");
        if (!outFile || outFile->IsZombie()) {
            throw std::runtime_error("Error: Could not create output file.");
        }
        std::cout << "Saving AnalysisPhaseSpace object to analysis_results.root..." << std::endl;
        
        outFile->WriteObject(&results, "analysis_phase_space");
        
        outFile->Close();
        delete outFile;
        std::cout << "Results saved successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}