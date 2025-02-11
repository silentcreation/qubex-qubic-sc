using namespace QPI;

constexpr uint64 QUBEX_MAX_SUPPLY_OF_TOKEN = 18446744073709551615ULL;                // 2^64 - 1
constexpr uint32 QUBEX_LENGTH_OF_TOKEN_ADDRESS = 42u;
constexpr uint32 QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN = 65536u;

constexpr uint32 QUBEX_SUCCESS = 0;
constexpr uint32 QUBEX_OVERFLOW_MAX_NUMBER_OF_TOKEN = 1;
constexpr uint32 QUBEX_ISSUED_TOKEN = 2;
constexpr uint32 QUBEX_INSUFFICIENT_TOKEN = 3;
constexpr uint32 QUBEX_NOT_ISSUED_TOKEN = 4;

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
        uint64 tokenName;
        uint64 receivedAmount;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct receiveTokenFromEthereum_output
    {
        uint32 returnCode;
    };

    struct sendTokenToEthereum_input
    {
        uint64 tokenName;
        uint64 sendAmount;
        Array<uint8, 64> addressOfTokenInEthereum;
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

    uint32 numberOfIssuedToken;

    struct ERC20TokenInfo
    {
        uint64 tokenName;
        sint64 numberOfShares;
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

    struct receiveTokenFromEthereum_locals
    {
        ERC20TokenInfo newAsset;
        uint32 _t, _r;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(receiveTokenFromEthereum)

        if(qpi.invocationReward() > 0)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
        }

        for(locals._t = 0 ; locals._t < state.numberOfIssuedToken; locals._t++)
        {
            for(locals._r = 0 ; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
            {
                if(state.wrappedTokens.get(locals._t).addressOfTokenInEthereum.get(locals._r) != input.addressOfTokenInEthereum.get(locals._r))
                {
                    break;
                }
            }
            if(locals._r == QUBEX_LENGTH_OF_TOKEN_ADDRESS)
            {
                break;
            }
        }

        locals.newAsset.tokenName = input.tokenName;
        for(locals._r = 0; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
        {
            locals.newAsset.addressOfTokenInEthereum.set(locals._r, input.addressOfTokenInEthereum.get(locals._r));
        }

        if(locals._t == state.numberOfIssuedToken)
        {
            if(state.numberOfIssuedToken + 1 >= QUBEX_MAX_SUPPLY_OF_TOKEN)
            {
                output.returnCode = QUBEX_OVERFLOW_MAX_NUMBER_OF_TOKEN;
                return ;
            }

            for(locals._r = 0; locals._r < state.numberOfIssuedToken; locals._r++)
            {
                if(state.wrappedTokens.get(locals._r).tokenName == input.tokenName)
                {
                    output.returnCode = QUBEX_ISSUED_TOKEN;
                    return ;
                }
            }
            qpi.issueAsset(input.tokenName, SELF, 0, QUBEX_MAX_SUPPLY_OF_TOKEN, 0);
            locals.newAsset.numberOfShares = input.receivedAmount;

            state.wrappedTokens.set(state.numberOfIssuedToken++, locals.newAsset);
        }
        else 
        {
            if(qpi.numberOfPossessedShares(input.tokenName, SELF, SELF, SELF, SELF_INDEX, SELF_INDEX) < input.receivedAmount)
            {
                output.returnCode = QUBEX_INSUFFICIENT_TOKEN;
                return ;
            }
            locals.newAsset.numberOfShares = state.wrappedTokens.get(locals._t).numberOfShares + input.receivedAmount;

            state.wrappedTokens.set(locals._t, locals.newAsset);
        }

        qpi.transferShareOwnershipAndPossession(input.tokenName, SELF, SELF, SELF, input.receivedAmount, qpi.invocator());

        output.returnCode = QUBEX_SUCCESS;

    _


    struct sendTokenToEthereum_locals
    {
        ERC20TokenInfo issuedAsset;
        uint32 _t, _r;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(sendTokenToEthereum)

        if(qpi.invocationReward() > 0)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
        }

        for(locals._t = 0 ; locals._t < state.numberOfIssuedToken; locals._t++)
        {
            for(locals._r = 0 ; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
            {
                if(state.wrappedTokens.get(locals._t).addressOfTokenInEthereum.get(locals._r) != input.addressOfTokenInEthereum.get(locals._r))
                {
                    break;
                }
            }
            if(locals._r == QUBEX_LENGTH_OF_TOKEN_ADDRESS)
            {
                break;
            }
        }

        if(locals._t == state.numberOfIssuedToken)
        {
            output.returnCode = QUBEX_NOT_ISSUED_TOKEN;
            return ;
        }

        if(qpi.numberOfPossessedShares(input.tokenName, SELF, qpi.invocator(), qpi.invocator(), SELF_INDEX, SELF_INDEX) < input.sendAmount)
        {
            output.returnCode = QUBEX_INSUFFICIENT_TOKEN;
            return ;
        }

        locals.issuedAsset.tokenName = state.wrappedTokens.get(locals._t).tokenName;
        locals.issuedAsset.numberOfShares -= input.sendAmount;
        for(locals._r = 0; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
        {
            locals.issuedAsset.addressOfTokenInEthereum.set(locals._r, input.addressOfTokenInEthereum.get(locals._r));
        }

        state.wrappedTokens.set(locals._t, locals.issuedAsset);

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