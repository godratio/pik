// Copyright 2019 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef CPIK_H_
#define CPIK_H_

#include "cmdline.h"
#include "codec.h"
#include "padded_bytes.h"
#include "pik_params.h"
#include "status.h"

namespace pik {

struct CompressArgs {
  // Initialize non-static default options.
  CompressArgs();

  // Add all the command line options to the CommandLineParser. Note that the
  // options are tied to the instance that this was called on.
  Status AddCommandLineOptions(tools::CommandLineParser* cmdline);

  // Validate the passed arguments, checking whether all passed options are
  // compatible. Returns whether the validation was successful.
  Status ValidateArgs(const tools::CommandLineParser& cmdline);


  DecoderHints dec_hints;
  CompressParams params;
  size_t num_threads = 0;
  bool got_num_threads = false;
  Override print_profile = Override::kDefault;

  // References (ids) of specific options to check if they were matched.
  tools::CommandLineParser::OptionId opt_distance_id = -1;
  tools::CommandLineParser::OptionId opt_target_size_id = -1;
  tools::CommandLineParser::OptionId opt_target_bpp_id = -1;
};

Status Compress(ThreadPool* pool, CompressArgs& args, PaddedBytes* compressed);

}  // namespace pik

#endif  // CPIK_H_
