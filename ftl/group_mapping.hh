#ifndef __FTL_GROUP_MAPPING__
#define __FTL_GROUP_MAPPING__

#include <cinttypes>
#include <unordered_map>
#include <vector>

#include "ftl/abstract_ftl.hh"
#include "ftl/common/block.hh"
#include "ftl/ftl.hh"
#include "pal/pal.hh"

namespace SimpleSSD {

namespace FTL {

class GroupMapping : public AbstractFTL {
 private:
  PAL::PAL *pPAL;

  ConfigReader &conf;
  // address mapping table: LPN（逻辑页）->vector<block index,page
  // index>（当bRandomTweak=false时是一一对应的） vector长度等于bitsetSize
  std::unordered_map<uint64_t, std::pair<uint32_t, uint32_t>> table;
  // 统计lpn已映射的subpage数量（递增）
  std::unordered_map<uint64_t, Bitset> groupUsedIoUnit;
  // （映射中的）block index 映射的Block instance
  std::unordered_map<uint32_t, Block> blocks;
  // 空闲物理块列表
  std::list<Block> freeBlocks;
  // 空闲的物理块数量
  uint32_t nFreeBlocks;  // For some libraries which std::list::size() is O(n)
  // vector长度等于可以充分并行的page数量（pageCountToMaxPerf=5），优先存放可以并发的块号
  std::vector<uint32_t> lastFreeBlock;
  uint32_t lastFreeBlockIndex;//这是数组下标
  std::unordered_map<uint64_t, std::vector<uint32_t>> requestCnt;

  bool bReclaimMore;
  bool bRandomTweak;    // true
  uint32_t bitsetSize;  // 96(bRandomTweak ? param.ioUnitInPage : 1)

  struct {
    uint64_t gcCount;
    uint64_t reclaimedBlocks;
    uint64_t validSuperPageCopies;
    uint64_t validPageCopies;
  } stat;

  float freeBlockRatio();
  uint32_t convertBlockIdx(uint32_t);
  // Search block which is blockIdx % param.pageCountToMaxPerf == idx；
  // if not found then first blockIdx
  uint32_t getFreeBlock(uint32_t);
  uint32_t getLastFreeBlock();
  void calculateVictimWeight(std::vector<std::pair<uint32_t, float>> &,
                             const EVICT_POLICY, uint64_t);
  void selectVictimBlock(std::vector<uint32_t> &, uint64_t &);
  void doGarbageCollection(std::vector<uint32_t> &, uint64_t &);

  float calculateWearLeveling();

  // 返回valid和invalid page的占比，用于报告盘状态
  void calculateTotalPages(uint64_t &, uint64_t &);

  void readInternal(Request &, uint64_t &);
  void writeInternal(Request &, uint64_t &, bool = true);
  void trimInternal(Request &, uint64_t &);
  void eraseInternal(PAL::Request &, uint64_t &);

 public:
  GroupMapping(ConfigReader &, Parameter &, PAL::PAL *, DRAM::AbstractDRAM *);
  ~GroupMapping();

  bool initialize() override;

  void read(Request &, uint64_t &) override;
  void write(Request &, uint64_t &) override;
  void trim(Request &, uint64_t &) override;

  void format(LPNRange &, uint64_t &) override;

  Status *getStatus(uint64_t, uint64_t) override;

  void getStatList(std::vector<Stats> &, std::string) override;
  /// @brief 获取状态（gcCount/reclaimedBlocks/validSuperPageCopies）等
  /// @param
  void getStatValues(std::vector<double> &) override;
  void resetStatValues() override;
};

}  // namespace FTL

}  // namespace SimpleSSD

#endif
