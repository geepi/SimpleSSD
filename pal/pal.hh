/*
 * Copyright (C) 2017 CAMELab
 *
 * This file is part of SimpleSSD.
 *
 * SimpleSSD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleSSD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleSSD.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PAL_PAL__
#define __PAL_PAL__

#include "util/def.hh"
#include "util/simplessd.hh"

namespace SimpleSSD {

namespace PAL {

class AbstractPAL;

typedef struct {
  uint32_t channel;          //!< Total # channels 12(intel750_400gb)
  uint32_t package;          //!< # packages / channel 5
  uint32_t die;              //!< # dies / package 8
  uint32_t plane;            //!< # planes / die 1
  uint32_t block;            //!< # blocks / plane 512
  uint32_t page;             //!< # pages / block 512
  uint32_t superBlock;       //!< Total super blocks 2560
  uint32_t pageSize;         //!< Size of page in bytes 4096
  uint32_t superPageSize;    //!< Size of super page in bytes 4096*96
  uint32_t pageInSuperPage;  //!< # pages in one superpage 96
} Parameter;

class PAL : public StatObject {
 private:
  Parameter param;
  AbstractPAL *pPAL;

  ConfigReader &conf;

 public:
  PAL(ConfigReader &);
  ~PAL();

  void read(Request &, uint64_t &);
  void write(Request &, uint64_t &);
  void erase(Request &, uint64_t &);
  void copyback(uint32_t, uint32_t, uint32_t, uint64_t &);

  Parameter *getInfo();

  void getStatList(std::vector<Stats> &, std::string) override;
  void getStatValues(std::vector<double> &) override;
  void resetStatValues() override;
};

}  // namespace PAL

}  // namespace SimpleSSD

#endif
