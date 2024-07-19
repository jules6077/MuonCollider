#include <iostream>
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "Math/Vector4D.h"
#include <sys/stat.h> // For mkdir() function

using namespace std;


// --- Histogram declaration

TH1F* h_truth_trhit_tim;
TH1F* h_truth_trhit_edp;

TH1F* h_reco_trhit_tim;
TH1F* h_reco_trhit_edp;

TH2F* h2d_truth_timVedp;
TH2F* h2d_reco_timVedp;

TH1F* invTmass; 
TH1F* invJmass;

//--Get the Name of Root File to Indicate the Layer level of interests---//
std::string stripExtension(const char* filename) {
    // Convert const char* to std::string
    std::string filenameStr(filename);
    
    // Find the position of the last '.' in the string
    size_t pos = filenameStr.find_last_of(".");
    
    if (pos != std::string::npos) {
        // Extract the substring before the last '.'
        return filenameStr.substr(0, pos);
    } else {
        // No '.' found, return the original filename
        return filenameStr;
    }
}

void trackerHit_analysis(const char *inputFile, const long int maxEvents=-1){
    
    TFile *file = new TFile(inputFile);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Unable to open input file: " << inputFile << std::endl;
        return;
    }
    else{cout << "The root file is: " << inputFile << endl;}
    
    TTree *myLCTuple = (TTree*) file->Get("MyLCTuple");
    if (!myLCTuple) {
    std::cerr << "Error: TTree not found or not properly initialized!" << std::endl;
    // Handle error or return
    return;
    }
    
    //--Get The Layer Name--//
    string layer = stripExtension(inputFile);

    // Create directory
    string dirname = Form("%s_plots", layer.c_str());
    int dir_status = mkdir(dirname.c_str(), 0777);

    //--Difine Histogram Windows--//
    h_truth_trhit_tim = new TH1F("h_truth_trhit_tim", Form("Simulated Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), 50, -0.2, 10);
    h_truth_trhit_edp = new TH1F("h_truth_trhit_edp", Form("Simulated Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), 50, 0, 0.12e-3);
    
    h_reco_trhit_tim = new TH1F("h_reco_trhit_tim", Form("Reconstructed Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), 50, -0.1, 11.5); // supposedly there is a cutoff time of 0.3 ns max for default digi tracks
    h_reco_trhit_edp = new TH1F("h_reco_trhit_edp", Form("Reconstructed Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), 50, 0, 0.12e-3);

    h2d_truth_timVedp = new TH2F("h2d_truth_timVedp", Form("Simulated Tracker Hits: Time vs Energy Deposited in %s;Time[ns];Energy Deposited [GeV]", layer.c_str()), 50, -0.001, 40, 50, 0, 0.00045); // time, energy
    h2d_reco_timVedp = new TH2F("h2d_reco_timVedp", Form("Reconstructed Tracker Hits: Time vs Energy Deposited in %s;Time[ns];Energy Deposited [GeV]", layer.c_str()), 50, -0.1, 11.5, 50, 0, 0.00075);
    
    invTmass = new TH1F("invTmass", Form("Simulated Track Hit Invariant Mass in %s; Inv. Mass [GeV]", layer.c_str()), 50, 0, 20);
    invJmass = new TH1F("invJmass", Form("Dijet Invariant Mass in %s; Inv. Mass [GeV]", layer.c_str()), 50, 20, 150);
    
    //--Harvest the leaves from the braches--//
    
    //--Simulated Tracker Hit--//
    int n_simHits;
    float *simHits_edp = new float[1500000];
    float *simHits_tim = new float[1500000];
    float *simHits_px = new float[1500000];
    float *simHits_py = new float[1500000];
    float *simHits_pz = new float[1500000];

    myLCTuple->SetBranchAddress("nsth", &n_simHits);
    myLCTuple->SetBranchAddress("stedp", simHits_edp);
    myLCTuple->SetBranchAddress("sttim", simHits_tim);
    myLCTuple->SetBranchAddress("stmox", simHits_px);
    myLCTuple->SetBranchAddress("stmoy", simHits_py);
    myLCTuple->SetBranchAddress("stmoz", simHits_pz);

    //--Reconstructed Tracker Hit--//
    int n_recHits;
    float *recHits_edp = new float[1500000];
    float *recHits_tim = new float[1500000];

    myLCTuple->SetBranchAddress("ntrh", &n_recHits);
    myLCTuple->SetBranchAddress("thedp", recHits_edp);
    myLCTuple->SetBranchAddress("thtim", recHits_tim);

    //--Jets disguised as isoleptons--//
    int n_jet;
    float *j_px = new float[1500000];
    float *j_py = new float[1500000];
    float *j_pz = new float[1500000];
    float *j_E = new float[1500000];
    float *j_Emiss = new float[1500000];

    myLCTuple->SetBranchAddress("niso", &n_jet);
    myLCTuple->SetBranchAddress("IsoPxvis", j_px);
    myLCTuple->SetBranchAddress("IsoPyvis", j_py);
    myLCTuple->SetBranchAddress("IsoPzvis", j_pz);
    myLCTuple->SetBranchAddress("IsoEvis", j_E);
    myLCTuple->SetBranchAddress("IsoEmiss", j_Emiss);


    // --- RECO jets
    // int n_jet;
    // float *jet_px  = new float[1000];
    // float *jet_py  = new float[1000];
    // float *jet_pz  = new float[1000];
    // float *jet_ene = new float[1000];
    // float *jet_q   = new float[1000];

    // myLCTuple->SetBranchAddress("njet", &n_jet);
    // myLCTuple->SetBranchAddress("jmox", j_px);
    // myLCTuple->SetBranchAddress("jmoy", j_py);
    // myLCTuple->SetBranchAddress("jmoz", j_pz);
    // myLCTuple->SetBranchAddress("jene", j_E);
    //myLCTuple->SetBranchAddress("jcha", jet_q);


    

    
    const long int nEntries = ( maxEvents < 0 ? myLCTuple->GetEntries() : maxEvents );
    for(int ientry=0; ientry<nEntries; ientry++){
        myLCTuple->GetEntry(ientry);
        //cout << "number of simHits: " << n_simHits << endl;
        
        int maxEindex = 0;
        int maxE = 0;
        for(int iTruth = 0; iTruth<n_simHits; iTruth++){
            //cout << "Loop iteration number: " << iTruth << endl;
            h_truth_trhit_edp->Fill(simHits_edp[iTruth]);
            h_truth_trhit_tim->Fill(simHits_tim[iTruth]);
            h2d_truth_timVedp->Fill(simHits_tim[iTruth], simHits_edp[iTruth]);
            //cout << "the edp is: " << simHits_edp[iTruth] << endl;
            //cout << "px: " << simHits_px[iTruth] << endl;
            //cout << "py: " << simHits_py[iTruth] << endl;
            //cout << "pz: " << simHits_pz[iTruth] << endl;

            if(simHits_edp[iTruth] >= maxE){
                maxEindex = iTruth;
            }
        }

        for(int iReco = 0; iReco<n_recHits; iReco++){
            h_reco_trhit_edp->Fill(recHits_edp[iReco]);
            h_reco_trhit_tim->Fill(recHits_tim[iReco]);
            h2d_reco_timVedp->Fill(recHits_tim[iReco], recHits_edp[iReco]);
        }

        ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > p_s1(abs(simHits_px[maxEindex]),abs(simHits_py[maxEindex]),abs(simHits_pz[maxEindex]),simHits_edp[maxEindex]);
        //ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > p_j2(jet_px[1],jet_py[1],jet_pz[1],jet_ene[1]);

        float simHits_mass = p_s1.M();
        invTmass->Fill(abs(simHits_mass));
        //cout << "Mass for leading particle: " << simHits_mass << endl;

        //if(n_jet == 1) cout << "there is a jet at event number " << ientry + 1 << endl; 
        //if(n_jet == 0) cout << "there is not a jet at event number " << ientry + 1 << endl;

        // --- select events with two reconstructed jets
        if ( n_jet != 2 ) continue; 

        double maxJene = -1.0;
        double maxJene2 = -1.0;
        int maxJen_index0 = -1;
        int maxJen_index1 = -1; 
        for(int ijet = 0; ijet < n_jet; ijet++){
            //find max jet: 
            if(j_E[ijet] > maxJene){
                maxJen_index0 = ijet;
                maxJene = j_E[ijet];
            }
            else if(j_E[ijet] < maxJene && j_E[ijet] > maxJene2){
                maxJen_index1 = ijet;
                maxJene2 = j_E[ijet];
            }
            
        }

        ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > p_j1(j_px[maxJen_index0],j_py[maxJen_index0],j_pz[maxJen_index0],j_E[maxJen_index0]);
        ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > p_j2(j_px[maxJen_index1],j_py[maxJen_index1],j_pz[maxJen_index1],j_E[maxJen_index1]);

        ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > p_j1m(j_px[maxJen_index0],j_py[maxJen_index0],j_pz[maxJen_index0],j_Emiss[maxJen_index0]);
        ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > p_j2m(j_px[maxJen_index1],j_py[maxJen_index1],j_pz[maxJen_index1],j_Emiss[maxJen_index1]);


        float dijet_mass = (p_j1+p_j2 + p_j1m+p_j2m).M(); // + (p_j1m+p_j2m).M();

        invJmass->Fill(dijet_mass);

        if ( ientry % 100 == 0 ) std::cout << ientry << " / " << nEntries << endl;
    
    }

    //--Create Your TCanvas--//-------------------------------------------------------------------------------------------------------------------
    TCanvas *canvas = new TCanvas("canvas", "Energy Deposited and Time Plots for Truth-Level and Reconstructed-Level Tracker Hits", 1200, 800);
    // Divide the canvas into a 2x2 grid of pads
    canvas->Divide(2, 2);
    
    canvas->cd(1);
    h_truth_trhit_edp->Draw();
    
    canvas->cd(2);
    h_truth_trhit_tim->Draw();

    canvas->cd(3);
    h_reco_trhit_edp->Draw();
    
    canvas->cd(4);
    h_reco_trhit_tim->Draw();

    canvas->SaveAs(Form("%s/edptimAll_%s.png", dirname.c_str(), layer.c_str()));
    // Close and delete the canvas
    canvas->Close();
    delete canvas;

    //--Create Another Canvas OverLapping Energy and Time------------------------------------------------------------------------------------------------
    // Create a TCanvas
    TCanvas *canvas1 = new TCanvas("canvas1", "Energy Deposited: Simulated vs Reconstructed Tracker Hits", 800, 600);
     // Set line and fill colors for histograms
    h_truth_trhit_edp->SetLineColor(kBlue);
    //h_truth_trhit_edp->SetFillColorAlpha(kBlue, 0.3); // Set fill color with transparency (0.3)
    h_reco_trhit_edp->SetLineColor(kRed);
    //h_reco_trhit_edp->SetFillColorAlpha(kRed, 0.3); // Set fill color with transparency (0.3)
    // Draw histograms on the same pad
    h_truth_trhit_edp->Draw("hist");
    h_reco_trhit_edp->Draw("hist same");
    //TLegend:
    TLegend *legend1 = new TLegend(0.65, 0.7, 0.85, 0.85); // x1, y1, x2, y2
    legend1->AddEntry(h_truth_trhit_edp, "Truth Hits", "l"); // "f" for filled area
    legend1->AddEntry(h_reco_trhit_edp, "Reco Hits", "l"); // "f" for filled area
    legend1->SetBorderSize(0); // No border around legend
    legend1->Draw();
    //Title:
    h_truth_trhit_edp->SetTitle("Energy Deposited: Simulated vs Reconstructed Tracker Hits");
    h_truth_trhit_edp->GetXaxis()->SetTitle("Energy Deposited [GeV]");
    h_truth_trhit_edp->GetYaxis()->SetTitle("Events");

    h_truth_trhit_edp->SetStats(kFALSE); // Disable statistics box for hist1
    h_reco_trhit_edp->SetStats(kFALSE); // Disable statistics box for hist2

    canvas1->SaveAs(Form("%s/edpOverlap_%s.png", dirname.c_str(), layer.c_str()));
    canvas1->Close();
    delete canvas1;

    // Create a TCanvas for Time --------------------------------------------------------------------------------------------------------
    TCanvas *canvas2 = new TCanvas("canvas2", "Time of Arrival: Simulated vs Reconstructed Tracker Hits", 800, 600);
     // Set line and fill colors for histograms
    h_truth_trhit_tim->SetLineColor(kBlue);
    //h_truth_trhit_tim->SetFillColorAlpha(kBlue, 0.3); // Set fill color with transparency (0.3)
    h_reco_trhit_tim->SetLineColor(kRed);
    //h_reco_trhit_tim->SetFillColorAlpha(kRed, 0.3); // Set fill color with transparency (0.3)
    // Draw histograms on the same pad
    h_truth_trhit_tim->Draw("hist");
    h_reco_trhit_tim->Draw("hist same");
    //TLegend:
    TLegend *legend2 = new TLegend(0.65, 0.7, 0.85, 0.85); // x1, y1, x2, y2
    legend2->AddEntry(h_truth_trhit_tim, "Truth Hits", "l"); // "f" for filled area
    legend2->AddEntry(h_reco_trhit_tim, "Reco Hits", "l"); // "f" for filled area
    legend2->SetBorderSize(0); // No border around legend
    legend2->Draw();
    //Title:
    h_truth_trhit_tim->SetTitle(Form("Time of Arrival: Simulated vs Reconstructed Tracker Hits in %s", layer.c_str()));
    h_truth_trhit_tim->GetXaxis()->SetTitle("Time [ns]");
    h_truth_trhit_tim->GetYaxis()->SetTitle("Events");

    h_truth_trhit_tim->SetStats(kFALSE); // Disable statistics box for hist1
    h_reco_trhit_tim->SetStats(kFALSE);

    // Set x-axis limits
    double xmin = -0.2;
    double xmax = 11;
    h_truth_trhit_tim->GetXaxis()->SetRangeUser(xmin, xmax);
    h_reco_trhit_tim->GetXaxis()->SetRangeUser(xmin, xmax);

    canvas2->SaveAs(Form("%s/timOverlap_%s.png", dirname.c_str(), layer.c_str()));
    canvas2->Close();
    delete canvas2;

    //NOW draw the 2d histograms on same canvas:-------------------------------------------------------------------------------------------------
    // Create a TCanvas
    TCanvas *canvas3 = new TCanvas("canvas3", "2d histos", 1400, 600); // Changing width from 1200 to 1400
    // Draw histograms on the same pad
    canvas3->Divide(2, 1); // Divide canvas into 2 columns, 1 row

    h2d_truth_timVedp->SetMarkerSize(1.5); // Set marker size (default is 1.0)
    h2d_truth_timVedp->SetMarkerColor(kRed);
    h2d_reco_timVedp->SetMarkerSize(1.5); // Set marker size (default is 1.0)
    h2d_reco_timVedp->SetMarkerColor(kBlue);

    canvas3->cd(1); // Activate first pad
    h2d_truth_timVedp->Draw(); // Draw hist1 on the first pad

    canvas3->cd(2); // Activate second pad
    h2d_reco_timVedp->Draw(); // Draw hist2 on the second pad
    canvas3->SaveAs(Form("%s/edptim2d_%s.png", dirname.c_str(), layer.c_str()));
    canvas3->Close();
    delete canvas3;
    


    //file->Close();
    
}
