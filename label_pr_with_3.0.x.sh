TARGET_BRANCH="origin/branch-3.0"
FILTER_LABEL=$1
DRY_RUN="false"

if [ -z "$FILTER_LABEL" ]; then
    echo "Usage: $0 <filter_label>"
    exit 1
fi

PRS=`gh pr list --label "$FILTER_LABEL" -s all -L 500 | awk '{print $1}'`

COMMITS=()
PRS_NEED_MERGE=()

for PR in $PRS; do
    MATCH_MSG=`git --no-pager log $TARGET_BRANCH --oneline | grep "#$PR"`
    if [ -z "$MATCH_MSG" ]; then
        echo "PR $PR is not in $TARGET_BRANCH"

        MATCH_MSG=`git --no-pager log "origin/master" --oneline | grep "#$PR"`
        COMMIT_HASH=`echo $MATCH_MSG | awk '{print $1}'`
        if [ -z "$MATCH_MSG" ]; then
            echo "PR $PR is not in origin/master"
        else
            gh pr view $PR --repo apache/doris --json labels --jq '.labels[].name' | grep "3.0.*-merged"
            if [ $? -eq 1 ]; then
                echo "label $PR with dev/3.0.x"
                if [ ${DRY_RUN} == "false" ]; then
                    gh pr edit $PR --add-label "dev/3.0.x"
                fi
                COMMITS+=("$COMMIT_HASH")
                PRS_NEED_MERGE+=("$PR")
            fi
        fi
    fi
done

# iterate over COMMITS and PRS_NEED_MERGE

for i in "${!COMMITS[@]}"
do
    COMMIT=${COMMITS[$i]}
    PR=${PRS_NEED_MERGE[$i]}
    echo "COMMIT: ${COMMIT} - PR: ${PR}"
    if [ ${DRY_RUN} == "false" ]; then
        git cherry-pick $COMMIT
        if [ $? -eq 0 ]; then
            gh pr edit $PR --remove-label "dev/3.0.x" --add-label "dev/3.0.1-merged"
        fi
    fi
done