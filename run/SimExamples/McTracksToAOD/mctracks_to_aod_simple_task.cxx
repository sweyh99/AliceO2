// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "Framework/AnalysisTask.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/ConfigParamSpec.h"

using namespace o2;
using namespace o2::framework;

void customize(std::vector<o2::framework::ConfigParamSpec>& workflowOptions)
{
  workflowOptions.push_back(ConfigParamSpec{"on-the-fly", o2::framework::VariantType::Bool, false, {"set the inputs for on-the-fly analysis"}});
}

#include "Framework/runDataProcessing.h"

struct SimpleTask {

  HistogramRegistry registry{"registry", {}};

  void init(o2::framework::InitContext& ic)
  {
    registry.add<TH1>("NEvents", "NEvents", HistType::kTH1F, {{1, 0, 1}}, false);
    registry.add<TH1>("phi", "phi", HistType::kTH1F, {{100, 0, 2 * PI}}, false);
    registry.add<TH1>("eta", "eta", HistType::kTH1F, {{100, -5, 5}}, false);
    registry.add<TH1>("pt", "pt", HistType::kTH1F, {{100, 0, 5}}, false);
  }

  void process(aod::McCollision const&, aod::McParticles const& mcParticles)
  {
    registry.fill(HIST("NEvents"), 0.5);
    for (auto& mcparticle : mcParticles) {
      registry.fill(HIST("pt"), mcparticle.pt());
      registry.fill(HIST("eta"), mcparticle.eta());
      registry.fill(HIST("phi"), mcparticle.phi());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec specs;
  DataProcessorSpec dSpec = adaptAnalysisTask<SimpleTask>(cfgc, TaskName{"mctracks-to-aod-simple-task"});
  if (cfgc.options().get<bool>("on-the-fly")) {
    std::vector<InputSpec> inputs;
    inputs.emplace_back("McCollisions", "AOD", "MCCOLLISION", 0, Lifetime::Timeframe);
    inputs.emplace_back("McParticles", "AOD", "MCPARTICLE_001", 0, Lifetime::Timeframe);
    inputs.emplace_back("McParticles_001Extension", "AOD", "MCPARTICLE_001E", 0, Lifetime::Timeframe);
    dSpec.inputs = inputs;
  }
  specs.emplace_back(dSpec);
  return specs;
}
