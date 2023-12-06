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

#ifndef __FTL_FTL__
#define __FTL_FTL__

#include "dram/abstract_dram.hh"
#include "pal/pal.hh"
#include "util/simplessd.hh"

namespace SimpleSSD {

namespace FTL {

class AbstractFTL;

typedef struct {
  uint64_t totalPhysicalBlocks;  //!< (PAL::Parameter::superBlock)等于超级块的数量2560
  uint64_t totalLogicalBlocks; // 减除OverProvision后的物理块总数 1990
  uint64_t pagesInBlock;  //!< (PAL::Parameter::page) 512
  uint32_t pageSize;      //!< Mapping unit (PAL::Parameter::superPageSize) 4096*96=393216 超级页的大小
  uint32_t ioUnitInPage;  //!< # smallest I/O unit in one page 96 注释应该是说一个page最多可以分为多少个unit？
  uint32_t pageCountToMaxPerf;  //!< # pages to fully utilize internal parallism 5
} Parameter;

class FTL : public StatObject {
 private:
  Parameter param;
  PAL::PAL *pPAL;

  ConfigReader &conf;
  AbstractFTL *pFTL;
  DRAM::AbstractDRAM *pDRAM;

 public:
  FTL(ConfigReader &, DRAM::AbstractDRAM *);
  ~FTL();

  void read(Request &, uint64_t &);
  void write(Request &, uint64_t &);
  void trim(Request &, uint64_t &);

  void format(LPNRange &, uint64_t &);

  Parameter *getInfo();
  uint64_t getUsedPageCount(uint64_t, uint64_t);

  void getStatList(std::vector<Stats> &, std::string) override;
  void getStatValues(std::vector<double> &) override;
  void resetStatValues() override;
};

}  // namespace FTL

}  // namespace SimpleSSD

#endif
