using namespace QPI;

constexpr uint64 QUBEX_MAX_SUPPLY_OF_TOKEN = 18446744073709551615ULL;                // 2^64 - 1
constexpr uint32 QUBEX_LENGTH_OF_TOKEN_ADDRESS = 42u;
constexpr uint32 QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN = 65536u;
constexpr uint32 QUBEX_INITIAL_LP_AMOUNT = 1000u;

constexpr uint32 QUBEX_SUCCESS = 0;
constexpr uint32 QUBEX_INPUT_ERROR = 11;
constexpr uint32 QUBEX_OVERFLOW_MAX_NUMBER_OF_TOKEN = 1;
constexpr uint32 QUBEX_ISSUED_TOKEN = 2;
constexpr uint32 QUBEX_INSUFFICIENT_TOKEN = 3;
constexpr uint32 QUBEX_NOT_ISSUED_TOKEN = 4;
constexpr uint32 QUBEX_CREATED_POOL = 5;
constexpr uint32 QUBEX_INSUFFICIENT_FUND = 6;
constexpr uint32 QUBEX_NOT_TRANSFERRED = 7;
constexpr uint32 QUBEX_NOT_CREATED_POOL = 8;
constexpr uint32 QUBEX_WRONG_LP_TOKEN_NAME = 9;
constexpr uint32 QUBEX_WRONG_TOKEN_NAME = 10;

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
        uint64 ERC20TokenName;
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
        uint64 ERC20TokenName;
        uint64 receivedAmount;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct receiveTokenFromEthereum_output
    {
        uint32 returnCode;
    };

    struct sendTokenToEthereum_input
    {
        uint64 ERC20TokenName;
        uint64 sendAmount;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct sendTokenToEthereum_output
    {
        uint32 returnCode;
    };

    struct createLiquidityPool_input
    {
        uint64 LPTokenName;
        uint64 amountOfQubic;
        uint64 ERC20TokenName;
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
        uint64 ERC20TokenName;
        uint64 LPTokenName;
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        uint32 tokenId;
        uint32 LPTokenId;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct addLiquidityPool_output
    {
        uint32 returnCode;
    };

    struct withdrawLiquidity_input
    {
        uint64 amountOfLP;
        uint32 idOfLP;
    };

    struct withdrawLiquidity_output
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
    uint32 numberOfPool;

    struct ERC20TokenInfo
    {
        uint64 ERC20TokenName;
        sint64 numberOfShares;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    Array<ERC20TokenInfo, QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN> wrappedTokens;

    struct liquidityInfo
    {
        uint64 ERC20TokenName;
        uint64 LPTokenName;
        uint64 supplyOfLPToken;
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        uint32 fee;
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    Array<liquidityInfo, QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN> liquidityPools;

    inline static uint64 min(uint64 a, uint64 b)
    {
        return (a < b) ? a : b;
    }

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

        locals.newAsset.ERC20TokenName = input.ERC20TokenName;
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
                if(state.wrappedTokens.get(locals._r).ERC20TokenName == input.ERC20TokenName)
                {
                    output.returnCode = QUBEX_ISSUED_TOKEN;
                    return ;
                }
            }

            for(locals._r = 0 ; locals._r < state.numberOfPool; locals._r++)
            {
                if(state.liquidityPools.get(locals._r).LPTokenName == input.ERC20TokenName)
                {
                    output.returnCode = QUBEX_ISSUED_TOKEN;
                    return ;
                }
            }
            qpi.issueAsset(input.ERC20TokenName, SELF, 0, QUBEX_MAX_SUPPLY_OF_TOKEN, 0);
            locals.newAsset.numberOfShares = input.receivedAmount;

            state.wrappedTokens.set(state.numberOfIssuedToken++, locals.newAsset);
        }
        else 
        {
            if(qpi.numberOfPossessedShares(input.ERC20TokenName, SELF, SELF, SELF, SELF_INDEX, SELF_INDEX) < input.receivedAmount)
            {
                output.returnCode = QUBEX_INSUFFICIENT_TOKEN;
                return ;
            }
            locals.newAsset.numberOfShares = state.wrappedTokens.get(locals._t).numberOfShares + input.receivedAmount;

            state.wrappedTokens.set(locals._t, locals.newAsset);
        }

        qpi.transferShareOwnershipAndPossession(input.ERC20TokenName, SELF, SELF, SELF, input.receivedAmount, qpi.invocator());

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

        if(qpi.numberOfPossessedShares(input.ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), SELF_INDEX, SELF_INDEX) < input.sendAmount)
        {
            output.returnCode = QUBEX_INSUFFICIENT_TOKEN;
            return ;
        }

        if(qpi.transferShareOwnershipAndPossession(input.ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), input.sendAmount, SELF) != input.sendAmount)
        {
            output.returnCode = QUBEX_NOT_TRANSFERRED;
            return ;
        }

        locals.issuedAsset.ERC20TokenName = state.wrappedTokens.get(locals._t).ERC20TokenName;
        locals.issuedAsset.numberOfShares -= input.sendAmount;
        for(locals._r = 0; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
        {
            locals.issuedAsset.addressOfTokenInEthereum.set(locals._r, input.addressOfTokenInEthereum.get(locals._r));
        }

        state.wrappedTokens.set(locals._t, locals.issuedAsset);

    _

    struct createLiquidityPool_locals
    {
        liquidityInfo newLP;
        uint32 _t, _r;
    };

    PUBLIC_PROCEDURE(createLiquidityPool)

        if(qpi.invocationReward() > 0)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
        }

        for(locals._r = 0; locals._r < state.numberOfIssuedToken; locals._r++)
        {
            if(state.wrappedTokens.get(locals._r).ERC20TokenName == input.LPTokenName)
            {
                output.returnCode = QUBEX_ISSUED_TOKEN;
                return ;
            }
        }

        for(locals._r = 0 ; locals._r < state.numberOfPool; locals._r++)
        {
            if(state.liquidityPools.get(locals._r).LPTokenName == input.LPTokenName)
            {
                output.returnCode = QUBEX_ISSUED_TOKEN;
                return ;
            }
        }

        for(locals._t = 0 ; locals._t < state.numberOfPool; locals._t++)
        {
            for(locals._r = 0 ; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
            {
                if(state.liquidityPools.get(locals._t).addressOfTokenInEthereum.get(locals._r) != input.addressOfTokenInEthereum.get(locals._r))
                {
                    break;
                }
            }
            if(locals._r == QUBEX_LENGTH_OF_TOKEN_ADDRESS)
            {
                break;
            }
        }

        if(locals._t != state.numberOfPool)
        {
            output.returnCode = QUBEX_CREATED_POOL;
            return ;
        }

        if(qpi.invocationReward() < input.amountOfQubic)
        {
            output.returnCode = QUBEX_INSUFFICIENT_FUND;
            return ;
        }
        if(qpi.numberOfPossessedShares(input.ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), SELF_INDEX, SELF_INDEX) < input.amountOfERC20Token)
        {
            output.returnCode = QUBEX_INSUFFICIENT_TOKEN;
            return ;
        }

        if(qpi.transferShareOwnershipAndPossession(input.ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), input.amountOfERC20Token, SELF) != input.amountOfERC20Token)
        {
            output.returnCode = QUBEX_NOT_TRANSFERRED;
            return ;
        }
        if(qpi.invocationReward() > input.amountOfQubic)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward() - input.amountOfQubic);
        }

        qpi.issueAsset(input.LPTokenName, SELF, 0, QUBEX_MAX_SUPPLY_OF_TOKEN, 0);
        qpi.transferShareOwnershipAndPossession(input.LPTokenName, SELF, SELF, SELF, QUBEX_INITIAL_LP_AMOUNT, qpi.invocator());

        locals.newLP.supplyOfLPToken = QUBEX_INITIAL_LP_AMOUNT;
        locals.newLP.fee = input.fee;
        locals.newLP.LPTokenName = input.LPTokenName;
        locals.newLP.amountOfQubic = input.amountOfQubic;
        locals.newLP.amountOfERC20Token = input.amountOfERC20Token;
        locals.newLP.ERC20TokenName = input.ERC20TokenName;
        for(locals._r = 0 ; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r)
        {
            locals.newLP.addressOfTokenInEthereum.set(locals._r, input.addressOfTokenInEthereum.get(locals._r));
        }
        state.liquidityPools.set(state.numberOfPool++, locals.newLP);
    _

    struct addLiquidityPool_locals
    {
        liquidityInfo updatedPool;
        uint64 amountOfNewLPToken;
        uint32 _r;
    };

    PUBLIC_PROCEDURE(addLiquidityPool)

        for(locals._r = 0 ; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
        {
            if(state.wrappedTokens.get(input.tokenId).addressOfTokenInEthereum.get(locals._r) != input.addressOfTokenInEthereum.get(locals._r))
            {
                break;
            }
        }

        if(locals._r != QUBEX_LENGTH_OF_TOKEN_ADDRESS)
        {
            output.returnCode = QUBEX_INPUT_ERROR;
            return ;
        }

        for(locals._r = 0 ; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
        {
            if(state.liquidityPools.get(input.LPTokenId).addressOfTokenInEthereum.get(locals._r) != input.addressOfTokenInEthereum.get(locals._r))
            {
                break;
            }
        }

        if(locals._r != QUBEX_LENGTH_OF_TOKEN_ADDRESS)
        {
            output.returnCode = QUBEX_INPUT_ERROR;
            return ;
        }

        if(state.wrappedTokens.get(input.tokenId).ERC20TokenName != input.ERC20TokenName)
        {
            output.returnCode = QUBEX_WRONG_TOKEN_NAME;
            return ;
        }

        if(state.liquidityPools.get(input.LPTokenId).LPTokenName != input.LPTokenName)
        {
            output.returnCode = QUBEX_WRONG_LP_TOKEN_NAME;
            return ;
        }

        if(qpi.invocationReward() < input.amountOfQubic)
        {
            output.returnCode = QUBEX_INSUFFICIENT_FUND;
            return ;
        }

        if(qpi.numberOfPossessedShares(input.ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), SELF_INDEX, SELF_INDEX) != input.amountOfERC20Token)
        {
            output.returnCode = QUBEX_INSUFFICIENT_TOKEN;
            return ;
        }

        if(qpi.invocationReward() > input.amountOfQubic)
        {
            qpi.transfer(qpi.invocator(), qpi.invocationReward() - input.amountOfQubic);
        }

        locals.amountOfNewLPToken = div(input.amountOfQubic, state.liquidityPools.get(input.LPTokenId).amountOfQubic) * state.liquidityPools.get(input.LPTokenId).supplyOfLPToken;
        locals.amountOfNewLPToken = min(locals.amountOfNewLPToken, div(input.amountOfERC20Token, state.liquidityPools.get(input.LPTokenId).amountOfERC20Token) * state.liquidityPools.get(input.LPTokenId).supplyOfLPToken);

        qpi.transferShareOwnershipAndPossession(input.ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), input.amountOfERC20Token, SELF);
        qpi.transferShareOwnershipAndPossession(input.LPTokenName, SELF, SELF, SELF, locals.amountOfNewLPToken, qpi.invocator());

        locals.updatedPool.LPTokenName = state.liquidityPools.get(input.LPTokenId).LPTokenName;
        locals.updatedPool.amountOfQubic = state.liquidityPools.get(input.LPTokenId).amountOfQubic + input.amountOfQubic;
        locals.updatedPool.amountOfERC20Token = state.liquidityPools.get(input.LPTokenId).amountOfERC20Token + input.amountOfERC20Token;
        locals.updatedPool.fee = state.liquidityPools.get(input.LPTokenId).fee;
        locals.updatedPool.supplyOfLPToken = state.liquidityPools.get(input.LPTokenId).supplyOfLPToken + locals.amountOfNewLPToken;
        locals.updatedPool.ERC20TokenName = state.liquidityPools.get(input.LPTokenId).ERC20TokenName;
        
        for(locals._r = 0 ; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
        {
            locals.updatedPool.addressOfTokenInEthereum.set(locals._r, state.liquidityPools.get(input.LPTokenId).addressOfTokenInEthereum.get(locals._r));
        }

        state.liquidityPools.set(input.LPTokenId, locals.updatedPool);

        output.returnCode = QUBEX_SUCCESS;
    _

    struct withdrawLiquidity_locals
    {
        liquidityInfo updatedPool;
        uint64 ERC20TokenName;
        uint64 LPTokenName;
        uint64 amountOfQubicTransferred;
        uint64 amountOfERC20TokenTransferred;
        uint32 _r;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(withdrawLiquidity)

        locals.ERC20TokenName = state.liquidityPools.get(input.idOfLP).ERC20TokenName;
        locals.LPTokenName = state.liquidityPools.get(input.idOfLP).LPTokenName;

        if(qpi.numberOfPossessedShares(locals.LPTokenName, SELF, qpi.invocator(), qpi.invocator(), SELF_INDEX, SELF_INDEX) < input.amountOfLP)
        {
            output.returnCode = QUBEX_INSUFFICIENT_TOKEN;
            return ;
        }

        if(qpi.transferShareOwnershipAndPossession(locals.LPTokenName, SELF, qpi.invocator(), qpi.invocator(), input.amountOfLP, SELF) != input.amountOfLP)
        {
            output.returnCode = QUBEX_NOT_TRANSFERRED;
            return ;
        }

        locals.amountOfERC20TokenTransferred = div(state.liquidityPools.get(input.idOfLP).amountOfERC20Token * input.amountOfLP, state.liquidityPools.get(input.idOfLP).supplyOfLPToken);
        locals.amountOfQubicTransferred = div(state.liquidityPools.get(input.idOfLP).amountOfQubic * input.amountOfLP, state.liquidityPools.get(input.idOfLP).supplyOfLPToken);

        qpi.transfer(qpi.invocator(), locals.amountOfQubicTransferred);
        qpi.transferShareOwnershipAndPossession(locals.ERC20TokenName, SELF, SELF, SELF, locals.amountOfERC20TokenTransferred, qpi.invocator());

        locals.updatedPool.amountOfERC20Token = state.liquidityPools.get(input.idOfLP).amountOfERC20Token - locals.amountOfERC20TokenTransferred;
        locals.updatedPool.amountOfQubic = state.liquidityPools.get(input.idOfLP).amountOfQubic - locals.amountOfQubicTransferred;
        locals.updatedPool.fee = state.liquidityPools.get(input.idOfLP).fee;
        locals.updatedPool.LPTokenName = state.liquidityPools.get(input.idOfLP).LPTokenName;
        locals.updatedPool.ERC20TokenName = state.liquidityPools.get(input.idOfLP).ERC20TokenName;
        locals.updatedPool.supplyOfLPToken = state.liquidityPools.get(input.idOfLP).supplyOfLPToken - input.amountOfLP;
        
        for(locals._r = 0 ; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
        {
            locals.updatedPool.addressOfTokenInEthereum.set(locals._r, state.liquidityPools.get(input.idOfLP).addressOfTokenInEthereum.get(locals._r));
        }

        state.liquidityPools.set(input.idOfLP, locals.updatedPool);

        output.returnCode = QUBEX_SUCCESS;
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
        REGISTER_USER_PROCEDURE(withdrawLiquidity, 5);
        REGISTER_USER_PROCEDURE(swap, 6);
    _

    INITIALIZE
    _

};