#include <iostream>
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "Math/Vector4D.h"
#include <sys/stat.h> // For mkdir() function
#include <string>   // Include the <string> header for std::string
#include <cstdio>   // Include the <cstdio> header for Form

using namespace std;


// --- Histogram declaration
//Signal Histos
TH1F* h_truth_trhit_tim;
TH1F* h_truth_trhit_edp;
TH1F* h_reco_trhit_tim;
TH1F* h_reco_trhit_edp;
TH1F* h_digi_trhit_tim;
TH1F* h_digi_trhit_edp;
TH2F* h2d_truth_timVedp;
TH2F* h2d_reco_timVedp;
//BIB histos
TH1F* h_truthBIB_trhit_tim;
TH1F* h_truthBIB_trhit_edp;
TH1F* h_recoBIB_trhit_tim;
TH1F* h_recoBIB_trhit_edp;
TH1F* h_digiBIB_trhit_tim;
TH1F* h_digiBIB_trhit_edp;
TH2F* h2d_truthBIB_timVedp;
TH2F* h2d_recoBIB_timVedp;

//NORM
TH1F* h_truth_trhit_tim_norm;
TH1F* h_truth_trhit_edp_norm;
TH1F* h_reco_trhit_tim_norm;
TH1F* h_reco_trhit_edp_norm;
TH1F* h_digi_trhit_tim_norm;
TH1F* h_digi_trhit_edp_norm;
TH1F* h_truthBIB_trhit_tim_norm;
TH1F* h_truthBIB_trhit_edp_norm;
TH1F* h_recoBIB_trhit_tim_norm;
TH1F* h_recoBIB_trhit_edp_norm;
TH1F* h_digiBIB_trhit_tim_norm;
TH1F* h_digiBIB_trhit_edp_norm;

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

void plot_four_hists(TH1F *hist1, TH1F *hist2, TH1F *hist3, TH1F *hist4, string dirname_, string layer_, string type){
    TCanvas *canvas = new TCanvas("canvas", "Energy Deposited and Time Plots for Truth-Level and Reconstructed-Level Tracker Hits", 1200, 800);
    // Divide the canvas into a 2x2 grid of pads
    canvas->Divide(2, 2);
    
    canvas->cd(1);
    hist1->Draw();
    
    canvas->cd(2);
    hist2->Draw();

    canvas->cd(3);
    hist3->Draw();
    
    canvas->cd(4);
    hist4->Draw();

    // To prevent the canvas from popping up automatically, use batch mode
    gROOT->SetBatch(kTRUE); // Enable batch mode

    canvas->SaveAs(Form("%s/%s_%s.png", dirname_.c_str(), type.c_str(), layer_.c_str()));
    // Close and delete the canvas
    canvas->Close();
    delete canvas;
}

void plot_two_hists(TH2F *hist1, TH2F *hist2, string dirname_, string layer_, string type){
    TCanvas *canvas = new TCanvas("canvas", "2d histos", 1400, 600); // Changing width from 1200 to 1400
    // Draw histograms on the same pad
    canvas->Divide(2, 1); // Divide canvas into 2 columns, 1 row

    hist1->SetMarkerSize(1.5); // Set marker size (default is 1.0)
    hist1->SetMarkerColor(kRed);
    hist2->SetMarkerSize(1.5); // Set marker size (default is 1.0)
    hist2->SetMarkerColor(kBlue);

    // To prevent the canvas from popping up automatically, use batch mode
    gROOT->SetBatch(kTRUE); // Enable batch mode

    canvas->cd(1); // Activate first pad
    hist1->Draw(); // Draw hist1 on the first pad

    canvas->cd(2); // Activate second pad
    hist2->Draw(); // Draw hist2 on the second pad
    canvas->SaveAs(Form("%s/%s_%s.png", dirname_.c_str(), type.c_str(),layer_.c_str()));
    canvas->Close();
    delete canvas;
}

void plot_overlapped_2hists(TH1F *hist1, TH1F *hist2, string dirname_, string layer_, string plotTitle, string plotXtitle, string plotYtitle, string legendName1, string legendName2, string overLapType, bool setYLimits = false, double ymin = 0, double ymax = 0, bool events = false, int sigNum = 0, int sigBNum = 0, bool logY = false){
    TCanvas *canvas = new TCanvas("canvas", "Energy Deposited: Simulated vs Reconstructed Tracker Hits", 800, 600);
     // Set line and fill colors for histograms
    hist1->SetLineColor(kBlue);
    //hist1->SetFillColorAlpha(kBlue, 0.3); // Set fill color with transparency (0.3)
    hist2->SetLineColor(kRed);
    //hist2->SetFillColorAlpha(kRed, 0.3); // Set fill color with transparency (0.3)
    // Draw histograms on the same pad
    hist1->Draw("hist");
    hist2->Draw("hist same");

    // Set x-axis limits
    // double xmin = -0.2;
    // double xmax = 11;
    // hist1->GetXaxis()->SetRangeUser(xmin, xmax);
    // hist2->GetXaxis()->SetRangeUser(xmin, xmax);

    //TLegend:
    TLegend *legend = new TLegend(0.65, 0.7, 0.85, 0.85); // x1, y1, x2, y2
    legend->AddEntry(hist1, Form("%s", legendName1.c_str()), "l"); // "f" for filled area
    legend->AddEntry(hist2, Form("%s", legendName2.c_str()), "l"); // "f" for filled area
    legend->SetBorderSize(0); // No border around legend
    legend->SetTextSize(0.03);

    if (events){
        TLatex *latex = new TLatex(0.6, 0.65, Form("Signal Entries: %i", sigNum));
        TLatex *latex2 = new TLatex(0.6, 0.6, Form("Signal+BIB Entries: %i", sigBNum));
        latex->SetTextSize(0.03);
        latex->SetNDC();
        latex->Draw();
        latex2->SetTextSize(0.03);
        latex2->SetNDC();
        latex2->Draw();
    }
    legend->Draw();
    //Title:
    hist1->SetTitle(Form("%s%s", plotTitle.c_str(), layer_.c_str()));
    hist1->GetXaxis()->SetTitle(Form("%s", plotXtitle.c_str()));
    hist1->GetYaxis()->SetTitle(Form("%s", plotYtitle.c_str()));
    //hist1->SetTitleSize(0.05);

    hist1->SetStats(kFALSE); // Disable statistics box for hist1
    hist2->SetStats(kFALSE); // Disable statistics box for hist2

    if (setYLimits) {
        hist1->GetYaxis()->SetRangeUser(ymin, ymax);
    }

    if(logY){
        canvas->SetLogy();
    }

    // To prevent the canvas from popping up automatically, use batch mode
    gROOT->SetBatch(kTRUE); // Enable batch mode

    canvas->SaveAs(Form("%s/%s_%s.png", dirname_.c_str(), overLapType.c_str(),layer_.c_str()));
    canvas->Close();
    delete canvas;
}

void plot_hist(TH1F *hist, string plotTitle, string plotXtitle, string dirname_,string name){
    // Create a canvas
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 800, 600);
    // Draw the histogram on the canvas
    hist->Draw();
    hist->SetTitle(Form("%s", plotTitle.c_str()));
    hist->GetXaxis()->SetTitle(Form("%s", plotXtitle.c_str()));
    hist->GetYaxis()->SetTitle("Events");

    // To prevent the canvas from popping up automatically, use batch mode
    gROOT->SetBatch(kTRUE); // Enable batch mode

    canvas->SaveAs(Form("%s/%s.png", dirname_.c_str(), name.c_str()));
    canvas->Close();
    delete canvas;
    
}

void trackerHit_analysis(const char *inputFile, const char *inputFileBIB,const char *inputFileDigi, const char *inputFileDigiBIB, std::string layer,const long int maxEvents=-1){
    
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

    // SIGNAL + BIB FILE
    TFile *fileBIB = new TFile(inputFileBIB);
    if (!fileBIB || file->IsZombie()) {
        std::cerr << "Error: Unable to open input file: " << inputFileBIB << std::endl;
        return;
    }
    else{cout << "The root file is: " << inputFileBIB << endl;}
    
    TTree *myLCTupleBIB = (TTree*) fileBIB->Get("MyLCTuple");
    if (!myLCTupleBIB) {
    std::cerr << "Error: TTree not found or not properly initialized!" << std::endl;
    // Handle error or return
    return;
    }

    // DIGI
    TFile *fileDigi = new TFile(inputFileDigi);
    if (!fileDigi || file->IsZombie()) {
        std::cerr << "Error: Unable to open input file: " << inputFileDigi << std::endl;
        return;
    }
    else{cout << "The root file is: " << inputFileDigi << endl;}
    
    TTree *myLCTupleDigi = (TTree*) fileDigi->Get("MyLCTuple");
    if (!myLCTupleDigi) {
    std::cerr << "Error: TTree not found or not properly initialized!" << std::endl;
    // Handle error or return
    return;
    }

    // DIGI BIB
    TFile *fileDigiBIB = new TFile(inputFileDigiBIB);
    if (!fileDigiBIB || file->IsZombie()) {
        std::cerr << "Error: Unable to open input file: " << inputFileDigiBIB << std::endl;
        return;
    }
    else{cout << "The root file is: " << inputFileDigiBIB << endl;}
    
    TTree *myLCTupleDigiBIB = (TTree*) fileDigiBIB->Get("MyLCTuple");
    if (!myLCTupleDigiBIB) {
    std::cerr << "Error: TTree not found or not properly initialized!" << std::endl;
    // Handle error or return
    return;
    }
    
    //--Get The Layer Name--//
    //string layer = stripExtension(inputFile);

    // Create directory
    string dirname = Form("%s_plots", layer.c_str());
    int dir_status = mkdir(dirname.c_str(), 0777);

    int bins = 50;
    float minTim = -0.1; //ns
    float maxTim = 6; //ns
    float minEn = 0; //GeV
    float maxEn = 0.25e-3; //GeV

    //--Difine Histogram Windows--//
    h_truth_trhit_tim = new TH1F("h_truth_trhit_tim", Form("Simulated Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim);
    h_truth_trhit_edp = new TH1F("h_truth_trhit_edp", Form("Simulated Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h_reco_trhit_tim = new TH1F("h_reco_trhit_tim", Form("Reconstructed Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); // supposedly there is a cutoff time of 0.3 ns max for default digi tracks
    h_reco_trhit_edp = new TH1F("h_reco_trhit_edp", Form("Reconstructed Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h_digi_trhit_tim = new TH1F("h_digi_trhit_tim", Form("Digitized Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); // supposedly there is a cutoff time of 0.3 ns max for default digi tracks
    h_digi_trhit_edp = new TH1F("h_digi_trhit_edp", Form("Digitized Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    //-- BIB --//
    h_recoBIB_trhit_tim = new TH1F("h_recoBIB_trhit_tim", Form("Reconstructed Tracker Hits Time of Arriaval w/ 1 Percent BIB in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim);
    h_recoBIB_trhit_edp = new TH1F("h_recoBIB_trhit_edp", Form("Reconstructed Tracker Hits Energy Deposited w/ 1 Percent BIB in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h_digiBIB_trhit_tim = new TH1F("h_digiBIB_trhit_tim", Form("Digitized Tracker Hits Time of Arriaval w/ 1 Percent BIB in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); 
    h_digiBIB_trhit_edp = new TH1F("h_digiBIB_trhit_edp", Form("Digitized Tracker Hits Energy Deposited w/ 1 Percent BIB in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h_truthBIB_trhit_tim = new TH1F("h_truthBIB_trhit_tim", Form("Simulated Tracker Hits Time of Arriaval w/ 1 Percent BIB in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim);
    h_truthBIB_trhit_edp = new TH1F("h_truthBIB_trhit_edp", Form("Simulated Tracker Hits Energy Deposited w/ 1 Percent BIB in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h2d_truthBIB_timVedp = new TH2F("h2d_truthBIB_timVedp", Form("Simulated Tracker Hits w/ 1 Percent BIB: Time vs Energy Deposited in %s;Time[ns];Energy Deposited [GeV]", layer.c_str()), bins, minTim, maxTim, bins, minEn, maxEn); // time, energy
    h2d_recoBIB_timVedp = new TH2F("h2d_recoBIB_timVedp", Form("Reconstructed Tracker Hits w/ 1 Percent BIB: Time vs Energy Deposited in %s;Time[ns];Energy Deposited [GeV]", layer.c_str()), bins, minTim, maxTim, bins, minEn, maxEn);

    //-- 2D HISTOS --//
    h2d_truth_timVedp = new TH2F("h2d_truth_timVedp", Form("Simulated Tracker Hits: Time vs Energy Deposited in %s;Time[ns];Energy Deposited [GeV]", layer.c_str()), bins, minTim, maxTim, bins, minEn, maxEn); // time, energy
    h2d_reco_timVedp = new TH2F("h2d_reco_timVedp", Form("Reconstructed Tracker Hits: Time vs Energy Deposited in %s;Time[ns];Energy Deposited [GeV]", layer.c_str()), bins, minTim, maxTim, bins, minEn, maxEn);
    
    invTmass = new TH1F("invTmass", Form("Simulated Track Hit Invariant Mass in %s; Inv. Mass [GeV]", layer.c_str()), 50, 0, 20);
    invJmass = new TH1F("invJmass", Form("Dijet Invariant Mass in %s; Inv. Mass [GeV]", layer.c_str()), 50, 20, 150);
    
    //--normalized histos: 
    h_digiBIB_trhit_tim_norm = new TH1F("h_digiBIB_trhit_tim_norm", Form("Digitized Tracker Hits Time of Arriaval w/ 1 Percent BIB in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); // supposedly there is a cutoff time of 0.3 ns max for default digi tracks
    h_digiBIB_trhit_edp_norm = new TH1F("h_digiBIB_trhit_edp_norm", Form("Digitized Tracker Hits Energy Deposited w/ 1 Percent BIB in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h_recoBIB_trhit_tim_norm = new TH1F("h_recoBIB_trhit_tim_norm", Form("Reconstructed Tracker Hits Time of Arriaval w/ 1 Percent BIB in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); // supposedly there is a cutoff time of 0.3 ns max for default digi tracks
    h_recoBIB_trhit_edp_norm = new TH1F("h_recoBIB_trhit_edp_norm", Form("Reconstructed Tracker Hits Energy Deposited w/ 1 Percent BIB in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h_truthBIB_trhit_tim_norm = new TH1F("h_truthBIB_trhit_tim_norm", Form("Simulated Tracker Hits Time of Arriaval w/ 1 Percent BIB in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim);
    h_truthBIB_trhit_edp_norm = new TH1F("h_truthBIB_trhit_edp_norm", Form("Simulated Tracker Hits Energy Deposited w/ 1 Percent BIB in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);

    h_truth_trhit_tim_norm = new TH1F("h_truth_trhit_tim_norm", Form("Simulated Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim);
    h_truth_trhit_edp_norm = new TH1F("h_truth_trhit_edp_norm", Form("Simulated Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h_reco_trhit_tim_norm = new TH1F("h_reco_trhit_tim_norm", Form("Reconstructed Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); 
    h_reco_trhit_edp_norm = new TH1F("h_reco_trhit_edp_norm", Form("Reconstructed Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    h_digi_trhit_tim_norm = new TH1F("h_digi_trhit_tim_norm", Form("Digitized Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); 
    h_digi_trhit_edp_norm = new TH1F("h_digi_trhit_edp_norm", Form("Digitized Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);

    //--Harvest the leaves from the braches--//
    
    //--Simulated Tracker Hit: SINGAL--//
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

    //--Reconstructed Tracker Hit: SIGNAL --//
    int n_recHits;
    float *recHits_edp = new float[1500000];
    float *recHits_tim = new float[1500000];

    myLCTuple->SetBranchAddress("ntrh", &n_recHits);
    myLCTuple->SetBranchAddress("thedp", recHits_edp);
    myLCTuple->SetBranchAddress("thtim", recHits_tim);

    //--Simulated Tracker Hit: BIB + SIGNAL--//
    int n_simHitsBIB;
    float *simHitsBIB_edp = new float[1500000];
    float *simHitsBIB_tim = new float[1500000];
    float *simHitsBIB_px = new float[1500000];
    float *simHitsBIB_py = new float[1500000];
    float *simHitsBIB_pz = new float[1500000];

    myLCTupleBIB->SetBranchAddress("nsth", &n_simHitsBIB);
    myLCTupleBIB->SetBranchAddress("stedp", simHitsBIB_edp);
    myLCTupleBIB->SetBranchAddress("sttim", simHitsBIB_tim);
    myLCTupleBIB->SetBranchAddress("stmox", simHitsBIB_px);
    myLCTupleBIB->SetBranchAddress("stmoy", simHitsBIB_py);
    myLCTupleBIB->SetBranchAddress("stmoz", simHitsBIB_pz);

    //--Reconstructed Tracker Hit: BIB + SIGNAL--//
    int n_recHitsBIB;
    float *recHitsBIB_edp = new float[1500000];
    float *recHitsBIB_tim = new float[1500000];

    myLCTupleBIB->SetBranchAddress("ntrh", &n_recHitsBIB);
    myLCTupleBIB->SetBranchAddress("thedp", recHitsBIB_edp);
    myLCTupleBIB->SetBranchAddress("thtim", recHitsBIB_tim);

    //--------------- DIGI ---------------//
    //--Digitized Tracker Hit: SINGAL--//
    int n_digHits;
    float *digHits_edp = new float[1500000];
    float *digHits_tim = new float[1500000];

    myLCTupleDigi->SetBranchAddress("ntrh", &n_digHits);
    myLCTupleDigi->SetBranchAddress("thedp", digHits_edp);
    myLCTupleDigi->SetBranchAddress("thtim", digHits_tim);

    //--Digitized Tracker Hit: BIB + SIGNAL--//
    int n_digHitsBIB;
    float *digHitsBIB_edp = new float[1500000];
    float *digHitsBIB_tim = new float[1500000];

    myLCTupleDigiBIB->SetBranchAddress("ntrh", &n_digHitsBIB);
    myLCTupleDigiBIB->SetBranchAddress("thedp", digHitsBIB_edp);
    myLCTupleDigiBIB->SetBranchAddress("thtim", digHitsBIB_tim);

    
    //--Jets disguised as isoleptons: SIGNAL--//
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


    int numEvents_truth = 0;
    int numEvents_digi = 0;
    int numEvents_reco = 0;
    int numEvents_truthBIB = 0;
    int numEvents_digiBIB = 0;
    int numEvents_recoBIB = 0;
    
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

            h_truth_trhit_edp_norm->Fill(simHits_edp[iTruth]);
            h_truth_trhit_tim_norm->Fill(simHits_tim[iTruth]);

            if(simHits_edp[iTruth] >= maxE){
                maxEindex = iTruth;
            }
            numEvents_truth += 1;
        }

        for(int iReco = 0; iReco<n_recHits; iReco++){
            h_reco_trhit_edp->Fill(recHits_edp[iReco]);
            h_reco_trhit_tim->Fill(recHits_tim[iReco]);
            h2d_reco_timVedp->Fill(recHits_tim[iReco], recHits_edp[iReco]);

            h_reco_trhit_edp_norm->Fill(recHits_edp[iReco]);
            h_reco_trhit_tim_norm->Fill(recHits_tim[iReco]);
            numEvents_reco += 1;
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

    const long int nEntriesBIB = ( maxEvents < 0 ? myLCTupleBIB->GetEntries() : maxEvents );
    for(int ientry=0; ientry<nEntriesBIB; ientry++){
        myLCTupleBIB->GetEntry(ientry);
        //cout << "number of simHits: " << n_simHits << endl;
        
        for(int iTruth = 0; iTruth<n_simHitsBIB; iTruth++){
            //cout << "Loop iteration number: " << iTruth << endl;
            h_truthBIB_trhit_edp->Fill(simHitsBIB_edp[iTruth]);
            h_truthBIB_trhit_tim->Fill(simHitsBIB_tim[iTruth]);
            h2d_truthBIB_timVedp->Fill(simHitsBIB_tim[iTruth], simHitsBIB_edp[iTruth]);
            
            h_truthBIB_trhit_edp_norm->Fill(simHitsBIB_edp[iTruth]);
            h_truthBIB_trhit_tim_norm->Fill(simHitsBIB_tim[iTruth]);

            numEvents_truthBIB+=1;
        }

        for(int iReco = 0; iReco<n_recHitsBIB; iReco++){
            h_recoBIB_trhit_edp->Fill(recHitsBIB_edp[iReco]);
            h_recoBIB_trhit_tim->Fill(recHitsBIB_tim[iReco]);
            h2d_recoBIB_timVedp->Fill(recHitsBIB_tim[iReco], recHitsBIB_edp[iReco]);

            h_recoBIB_trhit_edp_norm->Fill(recHitsBIB_edp[iReco]);
            h_recoBIB_trhit_tim_norm->Fill(recHitsBIB_tim[iReco]);

            numEvents_recoBIB+=1;
        }

        if ( ientry % 100 == 0 ) std::cout << ientry << " / " << nEntriesBIB << endl;
    
    }

    const long int nEntriesDigi = ( maxEvents < 0 ? myLCTupleDigi->GetEntries() : maxEvents );
    for(int ientry=0; ientry<nEntriesDigi; ientry++){
        myLCTupleDigi->GetEntry(ientry);
        //cout << "number of simHits: " << n_simHits << endl;
        
        for(int iTruth = 0; iTruth<n_digHits; iTruth++){
            //cout << "Loop iteration number: " << iTruth << endl;
            h_digi_trhit_edp->Fill(digHits_edp[iTruth]);
            h_digi_trhit_tim->Fill(digHits_tim[iTruth]);
            h_digi_trhit_edp_norm->Fill(digHits_edp[iTruth]);
            h_digi_trhit_tim_norm->Fill(digHits_tim[iTruth]);

            numEvents_digi+=1; 
        }
        if ( ientry % 100 == 0 ) std::cout << ientry << " / " << nEntriesDigi << endl;
    
    }

    const long int nEntriesDigiBIB = ( maxEvents < 0 ? myLCTupleDigiBIB->GetEntries() : maxEvents );
    for(int ientry=0; ientry<nEntriesDigiBIB; ientry++){
        myLCTupleDigiBIB->GetEntry(ientry);
        //cout << "number of simHits: " << n_simHits << endl;
        
        for(int iTruth = 0; iTruth<n_digHitsBIB; iTruth++){
            //cout << "Loop iteration number: " << iTruth << endl;
            h_digiBIB_trhit_edp->Fill(digHitsBIB_edp[iTruth]);
            h_digiBIB_trhit_tim->Fill(digHitsBIB_tim[iTruth]);
            h_digiBIB_trhit_edp_norm->Fill(digHitsBIB_edp[iTruth]);
            h_digiBIB_trhit_tim_norm->Fill(digHitsBIB_tim[iTruth]);
            numEvents_digiBIB+=1;
        }
        if ( ientry % 100 == 0 ) std::cout << ientry << " / " << nEntriesDigiBIB << endl;
    
    }

    //Normalize the Sig+BIB and The signal: 
    // Calculate the integral (total area under the histogram)
    double integral_digiBIB_trhit_edp = h_digiBIB_trhit_edp_norm->Integral();
    double integral_digiBIB_trhit_tim = h_digiBIB_trhit_tim_norm->Integral("width");
    double integral_recoBIB_trhit_edp = h_recoBIB_trhit_edp_norm->Integral();
    double integral_recoBIB_trhit_tim = h_recoBIB_trhit_tim_norm->Integral("width");
    double integral_truthBIB_trhit_edp = h_truthBIB_trhit_edp_norm->Integral();
    double integral_truthBIB_trhit_tim = h_truthBIB_trhit_tim_norm->Integral("width");

    double integral_digi_trhit_edp = h_digi_trhit_edp_norm->Integral();
    double integral_digi_trhit_tim = h_digi_trhit_tim_norm->Integral("width");
    double integral_reco_trhit_edp = h_reco_trhit_edp_norm->Integral();
    double integral_reco_trhit_tim = h_reco_trhit_tim_norm->Integral("width");
    double integral_truth_trhit_edp = h_truth_trhit_edp_norm->Integral();
    double integral_truth_trhit_tim = h_truth_trhit_tim_norm->Integral("width");

    h_digiBIB_trhit_edp_norm->Scale(1.0 / integral_digiBIB_trhit_edp);
    h_digiBIB_trhit_tim_norm->Scale(1.0 / integral_digiBIB_trhit_tim);
    h_recoBIB_trhit_edp_norm->Scale(1.0 / integral_recoBIB_trhit_edp);
    h_recoBIB_trhit_tim_norm->Scale(1.0 / integral_recoBIB_trhit_tim);
    h_truthBIB_trhit_edp_norm->Scale(1.0 / integral_truthBIB_trhit_edp);
    h_truthBIB_trhit_tim_norm->Scale(1.0 / integral_truthBIB_trhit_tim);

    h_digi_trhit_edp_norm->Scale(1.0 / integral_digi_trhit_edp);
    h_digi_trhit_tim_norm->Scale(1.0 / integral_digi_trhit_tim);
    h_reco_trhit_edp_norm->Scale(1.0 / integral_reco_trhit_edp);
    h_reco_trhit_tim_norm->Scale(1.0 / integral_reco_trhit_tim);
    h_truth_trhit_edp_norm->Scale(1.0 / integral_truth_trhit_edp);
    h_truth_trhit_tim_norm->Scale(1.0 / integral_truth_trhit_tim);
    

    //Plot all four histos for Truth/Reco
    plot_four_hists(h_truth_trhit_edp,h_truth_trhit_tim, h_reco_trhit_edp, h_reco_trhit_tim, dirname, layer, "edptimAll"); 
    //Plot energy deposited Truth/Reco Overlap
    plot_overlapped_2hists(h_truth_trhit_edp, h_reco_trhit_edp, dirname, layer, "Energy Deposited: Simulated vs Reconstructed Tracker Hits in ", "Energy Deposited [GeV]", "Events", "Truth Hits", "Reconstructed Hits", "edpOverlap");
    //Plot time of arrive truth/reco overlap
    plot_overlapped_2hists(h_truth_trhit_edp, h_reco_trhit_edp, dirname, layer, "Time of Arrival: Simulated vs Reconstructed Tracker Hits in ", "Time [ns]", "Events", "Truth Hits", "Reconstructed Hits", "timOverlap");
    //Plot the 2d histos for energy vs time
    plot_two_hists(h2d_truth_timVedp, h2d_reco_timVedp, dirname, layer, "edptim2d");

    //Plot all four histos for Truth/Reco
    plot_four_hists(h_truthBIB_trhit_edp,h_truthBIB_trhit_tim, h_recoBIB_trhit_edp, h_recoBIB_trhit_tim, dirname, layer, "edptimAllBIB"); 
    //Plot energy deposited Truth/Reco Overlap
    plot_overlapped_2hists(h_truthBIB_trhit_edp, h_recoBIB_trhit_edp, dirname, layer, "Energy Deposited: Simulated vs Reconstructed Tracker Hits in ", "Energy Deposited [GeV]", "Events", "Truth Hits", "Reconstructed Hits", "edpOverlapBIB");
    //Plot time of arrive truth/reco overlap
    plot_overlapped_2hists(h_truthBIB_trhit_edp, h_recoBIB_trhit_edp, dirname, layer, "Time of Arrival: Simulated vs Reconstructed Tracker Hits in ", "Time [ns]", "Events", "Truth Hits", "Reconstructed Hits", "timOverlapBIB");
    //Plot the 2d histos for energy vs time
    plot_two_hists(h2d_truthBIB_timVedp, h2d_recoBIB_timVedp, dirname, layer, "edptim2dBIB");

    //Plot Signal vs Bkg (Reco) EDP and TIME
    plot_overlapped_2hists(h_reco_trhit_edp_norm, h_recoBIB_trhit_edp_norm, dirname, layer, "Normalized Energy Deposited\n Signal vs Signal+BIB Reco Hits in ", "Energy Deposited [GeV]", "Events", "Signal Hits","Signal+BIB Hits", "sigBIBedp_reco", true, 0, 0.3, true, numEvents_reco, numEvents_recoBIB);
    plot_overlapped_2hists(h_reco_trhit_tim_norm, h_recoBIB_trhit_tim_norm, dirname, layer, "Normalized Time of Arrival\n Signal vs Signal+BIB Reco Hits in ", "Time [ns]", "Events", "Signal Hits", "Signal+BIB Hits", "sigBIBtim_reco", true, 0, 2.3, true, numEvents_reco, numEvents_recoBIB);
    //Plot Signal vs Bkg (Truth) EDP and TIME
    plot_overlapped_2hists(h_truth_trhit_edp_norm, h_truthBIB_trhit_edp_norm, dirname, layer, "Normalized Energy Deposited\n Signal vs Signal+BIB Truth Hits in ", "Energy Deposited [GeV]", "Events", "Signal Hits","Signal+BIB Hits", "sigBIBedp_truth", true, 0, 0.3, true, numEvents_truth, numEvents_truthBIB);
    plot_overlapped_2hists(h_truth_trhit_tim_norm, h_truthBIB_trhit_tim_norm, dirname, layer, "Normalized Time of Arrival\n Signal vs Signal+BIB Truth Hits in ", "Time [ns]", "Events", "Signal Hits","Signal+BIB Hits", "sigBIBtim_truth", true, 0, 2.3, true, numEvents_truth, numEvents_truthBIB);
    //Plot Signal vs Bkg (DIGI) EDP and TIME
    plot_overlapped_2hists(h_digi_trhit_edp_norm, h_digiBIB_trhit_edp_norm, dirname, layer, "Normalized Energy Deposited\n Signal vs Signal+BIB Digi Hits in ", "Energy Deposited [GeV]", "Events", "Signal Hits", "Signal+BIB Hits", "sigBIBedp_digi", true, 0, 0.3, true, numEvents_digi, numEvents_digiBIB);
    plot_overlapped_2hists(h_digi_trhit_tim_norm, h_digiBIB_trhit_tim_norm, dirname, layer, "Normalized Time of Arrival\n Signal vs Signal+BIB Digi Hits in ", "Time [ns]", "Events", "Signal Hits","Signal+BIB Hits", "sigBIBtim_digi", true, 0, 2.3, true, numEvents_digi, numEvents_digiBIB);
    
    plot_hist(h_digi_trhit_edp_norm, "Testing Y lim for EDP DIGI", "EDP", dirname,"testing");

    // //Plot Signal vs Bkg (Reco) EDP and TIME
    // plot_overlapped_2hists(h_recoBIB_trhit_edp, h_reco_trhit_edp, dirname, layer, "Energy Deposited\n Signal vs Signal+BIB Reco Hits in ", "Energy Deposited [GeV]", "Events","Signal+BIB Hits", "Signal Hits", "sigBIBedp_reco_logy", false, 0, 0.3, true, numEvents_reco, numEvents_recoBIB, true);
    // plot_overlapped_2hists(h_recoBIB_trhit_tim, h_reco_trhit_tim, dirname, layer, "Time of Arrival\n Signal vs Signal+BIB Reco Hits in ", "Time [ns]", "Events", "Signal+BIB Hits", "Signal Hits", "sigBIBtim_reco_logy", false, 0, 2.3, true, numEvents_reco, numEvents_recoBIB, true);
    // //Plot Signal vs Bkg (Truth) EDP and TIME
    // plot_overlapped_2hists(h_truthBIB_trhit_edp, h_truth_trhit_edp, dirname, layer, "Energy Deposited\n Signal vs Signal+BIB Truth Hits in ", "Energy Deposited [GeV]", "Events","Signal+BIB Hits", "Signal Hits", "sigBIBedp_truth_logy", false, 0, 0.3, true, numEvents_truth, numEvents_truthBIB, true);
    // plot_overlapped_2hists(h_truthBIB_trhit_tim, h_truth_trhit_tim, dirname, layer, "Time of Arrival\n Signal vs Signal+BIB Truth Hits in ", "Time [ns]", "Events", "Signal+BIB Hits", "Signal Hits", "sigBIBtim_truth_logy", false, 0, 2.3, true, numEvents_truth, numEvents_truthBIB, true);
    // //Plot Signal vs Bkg (DIGI) EDP and TIME
    // plot_overlapped_2hists(h_digiBIB_trhit_edp, h_digi_trhit_edp, dirname, layer, "Energy Deposited\n Signal vs Signal+BIB Digi Hits in ", "Energy Deposited [GeV]", "Events", "Signal+BIB Hits", "Signal Hits", "sigBIBedp_digi_logy", false, 0, 0.3, true, numEvents_digi, numEvents_digiBIB, true);
    // plot_overlapped_2hists(h_digiBIB_trhit_tim, h_digi_trhit_tim, dirname, layer, "Time of Arrival\n Signal vs Signal+BIB Digi Hits in ", "Time [ns]", "Events","Signal+BIB Hits", "Signal Hits", "sigBIBtim_digi_logy", false, 0, 2.3, true, numEvents_digi, numEvents_digiBIB, true);

    //file->Close();
    
}
