#pragma once
#include <AddressableDevice.hpp>

class Bus;
class GamePak;

class RicohRP2C02 : public AddressableDevice
{
    std::unique_ptr<Bus> bus;
    std::shared_ptr<GamePak> cartridge;
protected:
    virtual void setByte(uint16_t addr, uint8_t data) override;
    virtual uint8_t getByte(uint16_t addr, bool readOnly) const override;
public:
    RicohRP2C02(const uint16_t size, const uint16_t addrBase, const uint16_t addrEnd);
};