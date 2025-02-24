using namespace QPI;

constexpr uint64 QUBEX_MAX_SUPPLY_OF_TOKEN = 18446744073709551615ULL;                // 2^64 - 1
constexpr uint32 QUBEX_LENGTH_OF_TOKEN_ADDRESS = 42u;
constexpr uint32 QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN = 65536u;
constexpr uint32 QUBEX_INITIAL_LP_AMOUNT = 1000u;
constexpr uint32 QUBEX_SHAREHOLDERS_FEE = 5;            // permile
constexpr uint32 QUBEX_PLATFORM_FEE = 20;            // permile

constexpr uint32 QUBEX_SUCCESS = 0;
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
constexpr uint32 QUBEX_INPUT_ERROR = 11;

struct QUBEX2
{
};

struct QUBEX : public ContractBase
{
public:

    struct fetchTokenInfoById_input
    {
        Array<uint8, 64> addressOfTokenInEthereum;
    };

    struct fetchTokenInfoById_output
    {
        uint64 ERC20TokenName;
        sint64 numberOfShares;
    };

    struct fetchLPInfoById_input
    {
        uint64 ERC20TokenName;
    };

    struct fetchLPInfoById_output
    {
        uint64 LPTokenName;
        uint64 supplyOfLPToken;
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        uint64 collectedFee;
        uint32 fee;
    };

    struct getStateOfQubex_input
    {

    };

    struct getStateOfQubex_output
    {
        uint64 earnedQubic;
        uint64 shareHoldersFee;
        uint32 numberOfIssuedToken;
        uint32 numberOfPool;
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
        uint32 ERC20TokenId;
        uint32 LPTokenId;
    };

    struct addLiquidityPool_output
    {
        uint32 returnCode;
    };

    struct withdrawLiquidity_input
    {
        uint64 amountOfLP;
        uint32 LPTokenId;
    };

    struct withdrawLiquidity_output
    {
        uint32 returnCode;
    };

    struct swap_input
    {
        uint64 amountOfQubic;
        uint64 amountOfERC20Token;
        bit typeOfSwap;               // 0 means that user want to swap Qubic to ERC20Token, 1 means that user want to swap ERC20Token to Qubic
        uint32 ERC20TokenId;
        uint32 LPTokenId;
    };

    struct swap_output
    {
        uint32 returnCode;
    };

protected:

    uint64 earnedQubic;
    uint64 shareHoldersFee;
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
        uint64 collectedFee;
        uint32 fee;
    };

    Array<liquidityInfo, QUBEX_MAX_NUMBER_OF_ETHEREUM_TOKEN> liquidityPools;

    inline static uint64 min(uint64 a, uint64 b)
    {
        return (a < b) ? a : b;
    }

    inline static void numberToString(uint64 num, Array<uint8, 32>& strNum, uint32& length)
    {
        if (num == 0) {
            strNum.set(0, '0');
            return;
        }

        Array<uint8, 32> temp;
        length = 0;

        while (num > 0) {
            temp.set(length++, (uint8)((num % 10) + 48));
            num /= 10;
        }

        for (uint32 j = 0; j < length; j++) {
            strNum.set(j, temp.get(length - j - 1));
        }
    }

    inline static uint64 stringToNumber(Array<uint8, 32> strNum, uint32 length)
    {
        uint64 number = 0;

        for (uint32 i = 0; i < length; i++) 
        {
            if (strNum.get(i) < 48 || strNum.get(i) > 57) 
            {
                return 0; 
            }
            number = number * 10 + (strNum.get(i) - 48);
        }

        return number;
    }

    inline static void bigMulti(uint64 a, uint64 b, Array<uint8, 32>& strResult, uint32& lenResult)
    {
        Array<uint8, 32> strA;
        Array<uint8, 32> strB;
        Array<uint32, 64> tempResult;

        uint32 lenA;
        uint32 lenB;

        numberToString(a, strA, lenA);
        numberToString(a, strB, lenB);
        
        lenResult = lenA + lenB;
        
        for (uint32 i = lenA - 1; i >= 0; i--) 
        {
            for (uint32 j = lenB - 1; j >= 0; j--) 
            {
                uint32 mul = (strA.get(i) - 48) * (strB.get(j) - 48);
                uint32 sum = mul + tempResult.get(i + j + 1);

                tempResult.set(i + j + 1, sum % 10);
                tempResult.set(i + j, tempResult.get(i + j) + sum / 10);
            }
        }

        uint32 index = 0, k = 0;
        while (index < lenResult && tempResult.get(index) == 0) 
        {
            index++;
        }
        
        if (index == lenResult) 
        {
            strResult.set(0, 48);
        } 
        else 
        {
            while (index < lenResult) 
            {
                strResult.set(k++, tempResult.get(index++) + 48);
            }
        }
    }

    inline static uint64 bigDiv(Array<uint8, 32> a, uint64 b, uint32 lenA) {
        
        if (b == 0) 
        {
            return;
        }
    
        Array<uint8, 32> strResult;
        Array<uint8, 32> quotient;
        uint32 qIndex = 0;
        uint32 remainder = 0;
    
        for (uint32 i = 0; i < lenA; i++) 
        {
            remainder = remainder * 10 + (a.get(i) - 48);
    
            uint32 digit = remainder / b;
            remainder = remainder % b;
    
            if (qIndex != 0 || digit > 0) 
            {
                quotient.set(qIndex++, digit + 48);
            }
        }
    
        if (qIndex == 0) 
        {
            strResult.set(0, 48);
        } 
        else 
        {
            for(uint32 i = 0 ; i < qIndex; i++)
            {
                strResult.set(i, quotient.get(i));
            }
        }

        return stringToNumber(strResult, qIndex);
    }

    struct fetchTokenInfoById_locals
    {
        uint32 _t, _r;
    };

    PUBLIC_FUNCTION_WITH_LOCALS(fetchTokenInfoById)

        for(locals._t = 0 ; locals._t < state.numberOfIssuedToken; locals._t++)
        {
            for(locals._r = 0; locals._r < QUBEX_LENGTH_OF_TOKEN_ADDRESS; locals._r++)
            {
                if(state.wrappedTokens.get(locals._t).addressOfTokenInEthereum.get(locals._r) != input.addressOfTokenInEthereum.get(locals._r))
                {
                    break;
                }
            }
            if(locals._r == QUBEX_LENGTH_OF_TOKEN_ADDRESS)
            {
                output.ERC20TokenName = state.wrappedTokens.get(locals._t).ERC20TokenName;
                output.numberOfShares = state.wrappedTokens.get(locals._t).numberOfShares;
                return ;
            }
        }
    _

    struct fetchLPInfoById_locals
    {
        uint32 _t;
    };

    PUBLIC_FUNCTION_WITH_LOCALS(fetchLPInfoById)

        for(locals._t = 0 ; locals._t < state.numberOfPool; locals._t++)
        {
            if(state.liquidityPools.get(locals._t).amountOfERC20Token == input.ERC20TokenName)
            {
                output.amountOfERC20Token = state.liquidityPools.get(locals._t).amountOfERC20Token;
                output.amountOfQubic = state.liquidityPools.get(locals._t).amountOfQubic;
                output.collectedFee = state.liquidityPools.get(locals._t).collectedFee;
                output.fee = state.liquidityPools.get(locals._t).fee;
                output.LPTokenName = state.liquidityPools.get(locals._t).LPTokenName;
                output.supplyOfLPToken = state.liquidityPools.get(locals._t).supplyOfLPToken;

                return ;
            }
        }
    _

    PUBLIC_FUNCTION(getStateOfQubex)

        output.earnedQubic = state.earnedQubic;
        output.numberOfIssuedToken = state.numberOfIssuedToken;
        output.numberOfPool = state.numberOfPool;
        output.shareHoldersFee = state.shareHoldersFee;

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
            if(state.wrappedTokens.get(locals._t).ERC20TokenName == input.ERC20TokenName)
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
            locals.issuedAsset.addressOfTokenInEthereum.set(locals._r, state.wrappedTokens.get(locals._t).addressOfTokenInEthereum.get(locals._r));
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

        if(input.fee > 1000)
        {
            output.returnCode = QUBEX_INPUT_ERROR;
            return ;
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
        locals.newLP.collectedFee = 0;
        state.liquidityPools.set(state.numberOfPool++, locals.newLP);
    _

    struct addLiquidityPool_locals
    {
        liquidityInfo updatedPool;
        uint64 amountOfNewLPToken;
        uint32 _r;
        uint32 lenResult;
        Array<uint8, 32> multiResult;
    };

    PUBLIC_PROCEDURE(addLiquidityPool)

        if(input.LPTokenId >= state.numberOfPool || input.ERC20TokenId >= state.numberOfIssuedToken)
        {
            output.returnCode = QUBEX_OVERFLOW_MAX_NUMBER_OF_TOKEN;
            return ;
        }

        if(state.wrappedTokens.get(input.ERC20TokenId).ERC20TokenName != input.ERC20TokenName)
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

        bigMulti(input.amountOfQubic, state.liquidityPools.get(input.LPTokenId).supplyOfLPToken, locals.multiResult, locals.lenResult);
        locals.amountOfNewLPToken = bigDiv(locals.multiResult, state.liquidityPools.get(input.LPTokenId).amountOfQubic, locals.lenResult);
        
        bigMulti(input.amountOfERC20Token, state.liquidityPools.get(input.LPTokenId).supplyOfLPToken, locals.multiResult, locals.lenResult);
        locals.amountOfNewLPToken = min(locals.amountOfNewLPToken, bigDiv(locals.multiResult, state.liquidityPools.get(input.LPTokenId).amountOfERC20Token, locals.lenResult));

        qpi.transferShareOwnershipAndPossession(input.ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), input.amountOfERC20Token, SELF);
        qpi.transferShareOwnershipAndPossession(input.LPTokenName, SELF, SELF, SELF, locals.amountOfNewLPToken, qpi.invocator());

        locals.updatedPool.LPTokenName = state.liquidityPools.get(input.LPTokenId).LPTokenName;
        locals.updatedPool.amountOfQubic = state.liquidityPools.get(input.LPTokenId).amountOfQubic + input.amountOfQubic;
        locals.updatedPool.amountOfERC20Token = state.liquidityPools.get(input.LPTokenId).amountOfERC20Token + input.amountOfERC20Token;
        locals.updatedPool.fee = state.liquidityPools.get(input.LPTokenId).fee;
        locals.updatedPool.supplyOfLPToken = state.liquidityPools.get(input.LPTokenId).supplyOfLPToken + locals.amountOfNewLPToken;
        locals.updatedPool.ERC20TokenName = state.liquidityPools.get(input.LPTokenId).ERC20TokenName;
        locals.updatedPool.collectedFee = state.liquidityPools.get(input.LPTokenId).collectedFee;

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
        uint32 lenResult;
        Array<uint8, 32> multiResult;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(withdrawLiquidity)

        locals.ERC20TokenName = state.liquidityPools.get(input.LPTokenId).ERC20TokenName;
        locals.LPTokenName = state.liquidityPools.get(input.LPTokenId).LPTokenName;

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

        bigMulti(state.liquidityPools.get(input.LPTokenId).amountOfERC20Token, input.amountOfLP, locals.multiResult, locals.lenResult);
        locals.amountOfERC20TokenTransferred = bigDiv(locals.multiResult, state.liquidityPools.get(input.LPTokenId).supplyOfLPToken, locals.lenResult);
        
        bigMulti(state.liquidityPools.get(input.LPTokenId).amountOfQubic, input.amountOfLP, locals.multiResult, locals.lenResult);
        locals.amountOfQubicTransferred = bigDiv(locals.multiResult, state.liquidityPools.get(input.LPTokenId).supplyOfLPToken, locals.lenResult);

        qpi.transfer(qpi.invocator(), locals.amountOfQubicTransferred);
        qpi.transferShareOwnershipAndPossession(locals.ERC20TokenName, SELF, SELF, SELF, locals.amountOfERC20TokenTransferred, qpi.invocator());

        locals.updatedPool.amountOfERC20Token = state.liquidityPools.get(input.LPTokenId).amountOfERC20Token - locals.amountOfERC20TokenTransferred;
        locals.updatedPool.amountOfQubic = state.liquidityPools.get(input.LPTokenId).amountOfQubic - locals.amountOfQubicTransferred;
        locals.updatedPool.fee = state.liquidityPools.get(input.LPTokenId).fee;
        locals.updatedPool.LPTokenName = state.liquidityPools.get(input.LPTokenId).LPTokenName;
        locals.updatedPool.ERC20TokenName = state.liquidityPools.get(input.LPTokenId).ERC20TokenName;
        locals.updatedPool.supplyOfLPToken = state.liquidityPools.get(input.LPTokenId).supplyOfLPToken - input.amountOfLP;
        locals.updatedPool.collectedFee = state.liquidityPools.get(input.LPTokenId).collectedFee;

        state.liquidityPools.set(input.LPTokenId, locals.updatedPool);

        output.returnCode = QUBEX_SUCCESS;
    _

    struct swap_locals
    {
        liquidityInfo updatedPool;
        uint64 ERC20TokenName;
        uint64 LPTokenName;
        uint64 realSwappedAmount;
        uint64 amountOfSwappedToken;
        uint64 transferredQubicAmount;
        uint32 lenResult;
        Array<uint8, 32> multiResult;
    };

    PUBLIC_PROCEDURE(swap)

        if(input.LPTokenId >= state.numberOfPool || input.ERC20TokenId >= state.numberOfIssuedToken)
        {
            output.returnCode = QUBEX_OVERFLOW_MAX_NUMBER_OF_TOKEN;
            return ;
        }

        if(state.liquidityPools.get(input.LPTokenId).ERC20TokenName != state.wrappedTokens.get(input.ERC20TokenId).ERC20TokenName)
        {
            output.returnCode = QUBEX_INPUT_ERROR;
            return ;
        }

        locals.ERC20TokenName = state.liquidityPools.get(input.LPTokenId).ERC20TokenName;
        locals.LPTokenName = state.liquidityPools.get(input.LPTokenId).LPTokenName;

        if(input.typeOfSwap == 0)
        {
            if(qpi.invocationReward() < input.amountOfQubic)
            {
                output.returnCode = QUBEX_INSUFFICIENT_FUND;
                return ;
            }

            if(qpi.invocationReward() > input.amountOfQubic)
            {
                qpi.transfer(qpi.invocator(), input.amountOfQubic - qpi.invocationReward());
            }

            state.earnedQubic += div(input.amountOfQubic * QUBEX_PLATFORM_FEE, 1000ULL);
            state.shareHoldersFee += div(input.amountOfQubic * QUBEX_SHAREHOLDERS_FEE, 1000ULL);
            locals.updatedPool.collectedFee = state.liquidityPools.get(input.LPTokenId).collectedFee + div(input.amountOfQubic * state.liquidityPools.get(input.LPTokenId).fee, 1000ULL);
            locals.realSwappedAmount = input.amountOfQubic - div(input.amountOfQubic * QUBEX_PLATFORM_FEE, 1000ULL) - div(input.amountOfQubic * QUBEX_SHAREHOLDERS_FEE, 1000ULL) - div(input.amountOfQubic * state.liquidityPools.get(input.LPTokenId).fee, 1000ULL);

            bigMulti(state.liquidityPools.get(input.LPTokenId).amountOfERC20Token, state.liquidityPools.get(input.LPTokenId).amountOfQubic, locals.multiResult, locals.lenResult);
            locals.amountOfSwappedToken = state.liquidityPools.get(input.LPTokenId).amountOfERC20Token - bigDiv(locals.multiResult, (state.liquidityPools.get(input.LPTokenId).amountOfQubic + input.amountOfQubic), locals.lenResult);
            locals.updatedPool.amountOfQubic = state.liquidityPools.get(input.LPTokenId).amountOfQubic + input.amountOfQubic;
            locals.updatedPool.amountOfERC20Token = state.liquidityPools.get(input.LPTokenId).amountOfERC20Token - locals.amountOfSwappedToken;
            qpi.transferShareOwnershipAndPossession(state.liquidityPools.get(input.LPTokenId).ERC20TokenName, SELF, SELF, SELF, locals.amountOfSwappedToken, qpi.invocator());
            locals.updatedPool.ERC20TokenName = state.liquidityPools.get(input.LPTokenId).ERC20TokenName;
            locals.updatedPool.fee = state.liquidityPools.get(input.LPTokenId).fee;
            locals.updatedPool.LPTokenName = state.liquidityPools.get(input.LPTokenId).LPTokenName;
            locals.updatedPool.supplyOfLPToken = state.liquidityPools.get(input.LPTokenId).supplyOfLPToken;

            state.liquidityPools.set(input.LPTokenId, locals.updatedPool);
        }
        else 
        {
            if(qpi.numberOfPossessedShares(state.liquidityPools.get(input.LPTokenId).ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), SELF_INDEX, SELF_INDEX) < input.amountOfERC20Token)
            {
                output.returnCode = QUBEX_INSUFFICIENT_TOKEN;
                return ;
            }

            qpi.transferShareOwnershipAndPossession(state.liquidityPools.get(input.LPTokenId).ERC20TokenName, SELF, qpi.invocator(), qpi.invocator(), input.amountOfERC20Token, SELF);

            bigMulti(state.liquidityPools.get(input.LPTokenId).amountOfQubic, state.liquidityPools.get(input.LPTokenId).amountOfERC20Token, locals.multiResult, locals.lenResult);
            locals.amountOfSwappedToken = state.liquidityPools.get(input.LPTokenId).amountOfQubic - bigDiv(locals.multiResult, (state.liquidityPools.get(input.LPTokenId).amountOfERC20Token + input.amountOfERC20Token), locals.lenResult);

            state.earnedQubic += div(locals.amountOfSwappedToken * QUBEX_PLATFORM_FEE, 1000ULL);
            state.shareHoldersFee += div(locals.amountOfSwappedToken * QUBEX_SHAREHOLDERS_FEE, 1000ULL);
            locals.updatedPool.collectedFee = state.liquidityPools.get(input.LPTokenId).collectedFee + div(locals.amountOfSwappedToken * state.liquidityPools.get(input.LPTokenId).fee, 1000ULL);
            locals.transferredQubicAmount = locals.amountOfSwappedToken - div(locals.amountOfSwappedToken * QUBEX_PLATFORM_FEE, 1000ULL) - div(locals.amountOfSwappedToken * QUBEX_SHAREHOLDERS_FEE, 1000ULL) - div(locals.amountOfSwappedToken * state.liquidityPools.get(input.LPTokenId).fee, 1000ULL);

            qpi.transfer(qpi.invocator(), locals.transferredQubicAmount);

            locals.updatedPool.amountOfERC20Token = state.liquidityPools.get(input.LPTokenId).amountOfERC20Token + input.amountOfERC20Token;
            locals.updatedPool.amountOfQubic = state.liquidityPools.get(input.LPTokenId).amountOfQubic - locals.amountOfSwappedToken;
            locals.updatedPool.ERC20TokenName = state.liquidityPools.get(input.LPTokenId).ERC20TokenName;
            locals.updatedPool.fee = state.liquidityPools.get(input.LPTokenId).fee;
            locals.updatedPool.LPTokenName = state.liquidityPools.get(input.LPTokenId).LPTokenName;
            locals.updatedPool.supplyOfLPToken = state.liquidityPools.get(input.LPTokenId).supplyOfLPToken;

            state.liquidityPools.set(input.LPTokenId, locals.updatedPool);
        }

        output.returnCode = QUBEX_SUCCESS;
    _


    REGISTER_USER_FUNCTIONS_AND_PROCEDURES

        REGISTER_USER_FUNCTION(fetchTokenInfoById, 1);
        REGISTER_USER_FUNCTION(fetchLPInfoById, 2);
        REGISTER_USER_FUNCTION(getStateOfQubex, 3);

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