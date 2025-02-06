using namespace QPI;

constexpr uint32 QUBEX_LENGTH_OF_TOKEN_ADDRESS = 42u;
constexpr uint32 QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN = 4194304U;

struct QUBEX2
{
};

struct QUBEX : public ContractBase
{
public:

    struct fetchTokenInfo_input
    {
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct fetchTokenInfo_output
    {
        id issuer;
        uint64 tokenName;
        sint64 numberOfShares;
        uint64 unitOfMeasurement;
        sint8 numberOfDecimalPlaces;
    };

    struct fetchLPInfo_input
    {
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct fetchLPInfo_output
    {
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        uint32 fee;
    };

    struct receiveTokenFromEthereum_input
    {
        uint64 receiveAmount;
        uint64 tokenName;
        sint64 numberOfShares;
        uint64 unitOfMeasurement;
        sint8 numberOfDecimalPlaces;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct receiveTokenFromEthereum_output
    {
        uint32 returnCode;
    };

    struct sendTokenToEthereum_input
    {
        uint64 sendAmount;
        uint64 tokenName;
        sint64 numberOfShares;
        uint64 unitOfMeasurement;
        sint8 numberOfDecimalPlaces;
        Array<uint8, 64> addressOfTokenInEthereum;
        Array<uint8, 64> ethereumAddressOfUser;
    };

    struct sendTokenToEthereum_output
    {
        uint32 returnCode;
    };

    struct createLiquidityPool_input
    {
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        uint32 fee;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct createLiquidityPool_output
    {
        uint32 returnCode;
    };

    struct addLiquidityPool_input
    {
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct addLiquidityPool_output
    {
        uint32 returnCode;
    };

    struct swap_input
    {
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        Array<uint8, 64> addressOfTokenInEthereum;
        bit typeOfSwap;
    };

    struct swap_output
    {
        uint32 returnCode;
    };

protected:

    struct ERC20TokenInfo
    {
        id issuer;
        uint64 tokenName;
        sint64 numberOfShares;
        uint64 unitOfMeasurement;
        sint8 numberOfDecimalPlaces;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    Array<ERC20TokenInfo, QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN> wrappedTokens;

    struct liquidityInfo
    {
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        uint32 fee;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    Array<liquidityInfo, QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN> liquidityPools;

    PUBLIC_FUNCTION(fetchTokenInfo)


    _

    PUBLIC_FUNCTION(fetchLPInfo)

    
    _

    PUBLIC_PROCEDURE(receiveTokenFromEthereum)


    _

    PUBLIC_PROCEDURE(sendTokenToEthereum)


    _

    PUBLIC_PROCEDURE(createLiquidityPool)


    _

    PUBLIC_PROCEDURE(addLiquidityPool)


    _

    PUBLIC_PROCEDURE(swap)


    _

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES

        REGISTER_USER_FUNCTION(fetchTokenInfo, 1);
        REGISTER_USER_FUNCTION(fetchLPInfo, 2);

        REGISTER_USER_PROCEDURE(receiveTokenFromEthereum, 1);
        REGISTER_USER_PROCEDURE(sendTokenToEthereum, 2);
        REGISTER_USER_PROCEDURE(createLiquidityPool, 3);
        REGISTER_USER_PROCEDURE(addLiquidityPool, 4);
        REGISTER_USER_PROCEDURE(swap, 5);
    _

    INITIALIZE
    _

};