LEFT_BRANCH_NAME=$1
RIGHT_BRANCH_BANE=$2

PRS_IN_LEFT=`git log --oneline origin/master | awk '{print $NF}' | grep "(" | cut -c 2- | rev | cut -c 2- | rev`

for PR in ${PRS_IN_LEFT}; do
    MATCH_MSG_LINES=`git --no-pager log --grep "$PR" | wc -l`
    if [ ${MATCH_MSG_LINES=} -eq 0 ]; then
        git log $LEFT_BRANCH_NAME --grep "$PR"
    fi
done


