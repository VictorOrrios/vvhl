#include <vvhl/Core/UUID.hpp>
#include <random>


namespace vvhl
{

static std::random_device s_randomDevice;
static std::mt19937_64 s_engine(s_randomDevice());
static std::uniform_int_distribution<uint64_t> s_distribution;

UUID::UUID()
    : m_uuid(s_distribution(s_engine))
{
}

UUID::UUID(uint64_t uuid)
    : m_uuid(uuid)
{
}

}