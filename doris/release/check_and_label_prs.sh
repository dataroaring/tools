TARGET_BRANCH="origin/branch-3.0"
LAST_TAG="3.0.0-rc11"
LAST_LABEL="dev/3.0.0-merged"
NEW_LABEL="dev/3.0.1-merged"

PRS=`gh pr list --label 'dev/3.0.x' -s all -L 100 | awk '{print $1}'`
TAG_HASH=`git rev-list -n 1 $LAST_TAG`

for PR in $PRS; do
    echo "Checking PR $PR"
    MATCH_MSG=`git --no-pager log $TARGET_BRANCH --oneline | grep "#$PR"`
    if [ -z "$MATCH_MSG" ]; then
        echo "PR $PR is not in $TARGET_BRANCH"
        continue
    fi

    MATCH_MSG_LINES=`echo $MATCH_MSG | wc -l`

    if [ ${MATCH_MSG_LINES} -eq 1 ]; then
        COMMIT_HASH=`echo $MATCH_MSG | awk '{print $1}'`
        git merge-base --is-ancestor $COMMIT_HASH $TAG_HASH
        IS_ANCESTOR=$?
        echo "COMMIT_HASH: $COMMIT_HASH ANCESTOR_HASH: $ANCESTOR_HASH TAG_HASH: $TAG_HASH"
        if [ ${IS_ANCESTOR} -eq 1 ]; then
            echo "label $PR with $NEW_LABEL"
            gh pr edit $PR --remove-label "dev/3.0.x" --add-label $NEW_LABEL
        elif [ ${IS_ANCESTOR} -eq 0 ]; then
            echo "label $PR with $LAST_LABEL"
            gh pr edit $PR --remove-label 'dev/3.0.x' --add-label $LAST_LABEL
        else
            echo "Error: git merge-base failed"
        fi
    fi
done
