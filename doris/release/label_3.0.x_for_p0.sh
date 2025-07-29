# bin/sh
LABELS=("p0_w", "p0_l", "p0_c", "p0_b")

for LABEL in "${LABELS[@]}"; do
    echo "Processing label $LABEL"
    sh label_pr_with_3.0.x.sh $LABEL
done
