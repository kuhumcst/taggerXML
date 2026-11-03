#!/usr/bin/env bash
# trainBrill.bash - train the resource files for the Brill tagger (taggerXML).
#
# Follows the procedure in doc/README.TRAINING. Uses the Perl scripts in
# Utilities/ and Learner_Code/ and the programs taggerXML and
# contextual-rule-learn in Bin_and_Data/ (build them with 'make all' in src/).
#
# Input : a manually tagged corpus, one sentence per line, tokens as word/TAG
# Output: in the output directory, the files taggerXML needs at runtime:
#           FINAL.LEXICON  BIGBIGRAMLIST  LEXRULEOUTFILE  CONTEXT-RULEFILE
#         plus intermediate files, logs and an evaluation on held-out data.
#
# Usage: trainBrill.bash [options] TAGGED-CORPUS
# Run with -h for the options.

set -euo pipefail

SCRIPTDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

# ---------------------------------------------------------------- defaults
OUTDIR=./brill-model
BRILLDIR=${BRILLDIR:-$SCRIPTDIR}
TAGGER=${TAGGER:-}
CRL=${CRL:-}
PERL=${PERL:-perl}
NOUN=N_INDEF_SING     # start-state tag for unknown lower case words (taggerXML -n, lexical learners)
PROPER=EGEN           # start-state tag for other unknown words      (taggerXML -p, lexical learners)
TOPN=300              # unknown-lexical-learn: # of most frequent words used to filter bigrams
MINBIGRAM=0           # keep only bigrams occurring more than this many times
SEED=                 # random seed for the corpus split (empty = random)
RESUME=0
FORCE=0

usage() {
    cat <<EOF
Usage: $(basename "$0") [options] TAGGED-CORPUS

Trains Brill tagger files from a tagged corpus (one sentence per line,
tokens written as word/TAG).

Options:
  -o DIR    output directory                          (default: $OUTDIR)
  -B DIR    directory with Utilities/, Learner_Code/ and Bin_and_Data/
                                                      (default: $BRILLDIR)
  -t PATH   taggerXML executable                      (default: BRILLDIR/Bin_and_Data/
                                                       taggerXML or in PATH)
  -c PATH   contextual-rule-learn executable          (default: BRILLDIR/Bin_and_Data/
                                                       contextual-rule-learn or in PATH)
  -n TAG    start-state tag for unknown words starting lower case
                                                      (default: $NOUN)
  -p TAG    start-state tag for other unknown words   (default: $PROPER)
            Used both by the lexical rule learners and by taggerXML.
  -N NUM    top-N frequent words for lexical learning (default: $TOPN)
  -m NUM    drop bigrams occurring NUM times or less  (default: $MINBIGRAM = keep all)
  -s SEED   random seed for splitting the corpus in two halves
  -r        resume: skip steps whose output already exists and continue
            interrupted lexical rule learning where it stopped
  -f        overwrite an existing, non-empty output directory
  -h        show this help

Environment variables BRILLDIR, TAGGER, CRL and PERL can also be used.
EOF
}

die()  { echo "ERROR: $*" >&2; exit 1; }
log()  { echo "[$(date '+%F %T')] $*" >&2; }

while getopts "o:B:t:c:n:p:N:m:s:rfh" opt; do
    case $opt in
        o) OUTDIR=$OPTARG ;;
        B) BRILLDIR=$OPTARG ;;
        t) TAGGER=$OPTARG ;;
        c) CRL=$OPTARG ;;
        n) NOUN=$OPTARG ;;
        p) PROPER=$OPTARG ;;
        N) TOPN=$OPTARG ;;
        m) MINBIGRAM=$OPTARG ;;
        s) SEED=$OPTARG ;;
        r) RESUME=1 ;;
        f) FORCE=1 ;;
        h) usage; exit 0 ;;
        *) usage >&2; exit 2 ;;
    esac
done
shift $((OPTIND - 1))
[ $# -eq 1 ] || { usage >&2; exit 2; }

# ---------------------------------------------------------------- checks
CORPUS=$(readlink -f "$1")
[ -s "$CORPUS" ] || die "tagged corpus '$1' not found or empty"

BRILLDIR=$(readlink -f "$BRILLDIR")
UTIL=$BRILLDIR/Utilities
LEARN=$BRILLDIR/Learner_Code
for f in divide-in-two-rand.prl tagged-to-untagged.prl wordlist-make.prl \
         word-tag-count.prl bigram-generate.prl make-restricted-lexicon.prl; do
    [ -f "$UTIL/$f" ] || die "missing $UTIL/$f (use -B)"
done
[ -f "$LEARN/unknown-lexical-learn.prl" ] || die "missing $LEARN/unknown-lexical-learn.prl (use -B)"

if [ -z "$TAGGER" ]; then
    if [ -x "$BRILLDIR/Bin_and_Data/taggerXML" ]; then
        TAGGER=$BRILLDIR/Bin_and_Data/taggerXML
    else
        TAGGER=$(command -v taggerXML || true)
    fi
fi
[ -n "$TAGGER" ] && [ -x "$TAGGER" ] || die "taggerXML executable not found (use -t)"
TAGGER=$(readlink -f "$TAGGER")

if [ -z "$CRL" ]; then
    if [ -x "$BRILLDIR/Bin_and_Data/contextual-rule-learn" ]; then
        CRL=$BRILLDIR/Bin_and_Data/contextual-rule-learn
    else
        CRL=$(command -v contextual-rule-learn || true)
    fi
fi
[ -n "$CRL" ] && [ -x "$CRL" ] || die "contextual-rule-learn executable not found (use -c)"
CRL=$(readlink -f "$CRL")

command -v "$PERL" >/dev/null || die "perl not found"
[[ $TOPN =~ ^[0-9]+$ ]]      || die "-N needs a number"
[[ $MINBIGRAM =~ ^[0-9]+$ ]] || die "-m needs a number"

if [ -d "$OUTDIR" ] && [ -n "$(ls -A "$OUTDIR")" ] && [ $RESUME -eq 0 ] && [ $FORCE -eq 0 ]; then
    die "output directory '$OUTDIR' is not empty (use -f to overwrite or -r to resume)"
fi
mkdir -p "$OUTDIR"
cd "$OUTDIR"

# Sorting must be byte-based and locale independent.
export LC_ALL=C

# done_already FILE...: true if resuming and all FILEs exist and are non-empty
done_already() {
    [ $RESUME -eq 1 ] || return 1
    local f
    for f in "$@"; do [ -s "$f" ] || return 1; done
    log "  skipped (output exists)"
}

log "corpus      : $CORPUS ($(wc -l < "$CORPUS") lines, $(wc -w < "$CORPUS") tokens)"
log "output      : $(pwd)"
log "Brill dir   : $BRILLDIR"
log "taggerXML   : $TAGGER"
log "ctx learner : $CRL"
log "noun/proper : $NOUN / $PROPER"

# ---------------------------------------------------------------- 1. split
log "1. Splitting corpus into TAGGED-CORPUS and TAGGED-CORPUS-2"
if ! done_already TAGGED-CORPUS-ENTIRE TAGGED-CORPUS TAGGED-CORPUS-2; then
    # normalised copy of the input: no CR (DOS line ends), no empty lines
    tr -d '\r' < "$CORPUS" | awk 'NF' > TAGGED-CORPUS-ENTIRE
    if [ -n "$SEED" ]; then
        # reproducible version of divide-in-two-rand.prl
        awk -v seed="$SEED" 'BEGIN{srand(seed)}
             { if (rand() > .5) print > "TAGGED-CORPUS"; else print > "TAGGED-CORPUS-2" }' TAGGED-CORPUS-ENTIRE
    else
        "$PERL" "$UTIL/divide-in-two-rand.prl" TAGGED-CORPUS TAGGED-CORPUS-2 < TAGGED-CORPUS-ENTIRE
    fi
fi
log "   TAGGED-CORPUS: $(wc -w < TAGGED-CORPUS) tokens, TAGGED-CORPUS-2: $(wc -w < TAGGED-CORPUS-2) tokens"

# ---------------------------------------------------------------- 2. word lists
log "2. Making UNTAGGED-CORPUS, BIGWORDLIST, SMALLWORDTAGLIST, BIGBIGRAMLIST"
if ! done_already UNTAGGED-CORPUS BIGWORDLIST SMALLWORDTAGLIST BIGBIGRAMLIST; then
    "$PERL" "$UTIL/tagged-to-untagged.prl" < TAGGED-CORPUS-ENTIRE > UNTAGGED-CORPUS
    "$PERL" "$UTIL/wordlist-make.prl" < UNTAGGED-CORPUS \
        | sort -k2,2 -rn | awk '{ print $1 }' > BIGWORDLIST
    "$PERL" "$UTIL/word-tag-count.prl" < TAGGED-CORPUS \
        | sort -k3,3 -rn > SMALLWORDTAGLIST
    "$PERL" "$UTIL/bigram-generate.prl" < UNTAGGED-CORPUS \
        | awk -v min="$MINBIGRAM" '$3 > min { print $1, $2 }' > BIGBIGRAMLIST
fi

# ---------------------------------------------------------------- 3. lexical rules
#
# The learner writes "START: <date>" when it begins, one line per rule, and
# "SLUT: <date>" when it finishes. A rule file whose last line is not SLUT:
# comes from an interrupted run; with -r, learning is then continued with
# unknown-lexical-learn-continue.prl, which first applies the rules learned
# so far. While it runs, the earlier rules are kept in LEXRULEOUTFILE.old.

count_rules() { grep -cvE '^(START|SLUT):|^[[:space:]]*$' "$1" || true; }
lexrules_complete() {
    [ -s LEXRULEOUTFILE ] && grep -v '^[[:space:]]*$' LEXRULEOUTFILE | tail -n 1 | grep -q '^SLUT:'
}

log "3. Learning lexical rules for unknown words -> LEXRULEOUTFILE (slow)"
if [ $RESUME -eq 1 ] && [ -e LEXRULEOUTFILE.old ]; then
    # a continue run was interrupted: put the old and new rules back together
    # (unless that had already happened when the script was stopped)
    if ! { [ -f LEXRULEOUTFILE ] &&
           cmp -s -n "$(stat -c %s LEXRULEOUTFILE.old)" LEXRULEOUTFILE.old LEXRULEOUTFILE; }; then
        cat LEXRULEOUTFILE.old $( [ -f LEXRULEOUTFILE ] && echo LEXRULEOUTFILE ) > LEXRULEOUTFILE.tmp
        mv LEXRULEOUTFILE.tmp LEXRULEOUTFILE
    fi
    rm -f LEXRULEOUTFILE.old
fi

if [ $RESUME -eq 1 ] && lexrules_complete; then
    log "  skipped (output exists)"
elif [ $RESUME -eq 1 ] && [ "$(count_rules LEXRULEOUTFILE 2>/dev/null)" -gt 0 ] 2>/dev/null; then
    [ -f "$LEARN/unknown-lexical-learn-continue.prl" ] \
        || die "missing $LEARN/unknown-lexical-learn-continue.prl, cannot continue lexical learning"
    # drop a rule that was only half written when the learner was stopped
    if [ -n "$(tail -c 1 LEXRULEOUTFILE)" ]; then
        sed -i '$d' LEXRULEOUTFILE
    fi
    log "  continuing interrupted run after $(count_rules LEXRULEOUTFILE) rules"
    mv LEXRULEOUTFILE LEXRULEOUTFILE.old
    "$PERL" "$LEARN/unknown-lexical-learn-continue.prl" BIGWORDLIST SMALLWORDTAGLIST \
        BIGBIGRAMLIST "$TOPN" LEXRULEOUTFILE LEXRULEOUTFILE.old "$NOUN" "$PROPER" >> outlex 2>> errlex \
        || die "lexical rule learning failed, see $(pwd)/errlex (rerun with -r to continue)"
    cat LEXRULEOUTFILE.old LEXRULEOUTFILE > LEXRULEOUTFILE.tmp
    mv LEXRULEOUTFILE.tmp LEXRULEOUTFILE
    rm -f LEXRULEOUTFILE.old
else
    rm -f LEXRULEOUTFILE   # the learner appends
    "$PERL" "$LEARN/unknown-lexical-learn.prl" BIGWORDLIST SMALLWORDTAGLIST \
        BIGBIGRAMLIST "$TOPN" LEXRULEOUTFILE "$NOUN" "$PROPER" > outlex 2> errlex \
        || die "lexical rule learning failed, see $(pwd)/errlex (rerun with -r to continue)"
fi
log "   $(count_rules LEXRULEOUTFILE) lexical rules"

# ---------------------------------------------------------------- 4. lexicons
log "4. Making TRAINING.LEXICON and FINAL.LEXICON"
if ! done_already TRAINING.LEXICON FINAL.LEXICON UNTAGGED-CORPUS-2; then
    "$PERL" "$UTIL/make-restricted-lexicon.prl" < TAGGED-CORPUS > TRAINING.LEXICON
    "$PERL" "$UTIL/make-restricted-lexicon.prl" < TAGGED-CORPUS-ENTIRE > FINAL.LEXICON
    "$PERL" "$UTIL/tagged-to-untagged.prl" < TAGGED-CORPUS-2   > UNTAGGED-CORPUS-2
fi

# ---------------------------------------------------------------- 5. dummy tagging
log "5. Start-state tagging UNTAGGED-CORPUS-2 -> DUMMY-TAGGED-CORPUS"
if ! done_already DUMMY-TAGGED-CORPUS; then
    "$TAGGER" -n "$NOUN" -p "$PROPER" -w BIGWORDLIST -S \
        TRAINING.LEXICON UNTAGGED-CORPUS-2 BIGBIGRAMLIST LEXRULEOUTFILE \
        > DUMMY-TAGGED-CORPUS
fi

# ---------------------------------------------------------------- 6. contextual rules
log "6. Learning contextual rules -> CONTEXT-RULEFILE (slow)"
if ! done_already CONTEXT-RULEFILE outcontext; then
    rm -f CONTEXT-RULEFILE  # the learner appends
    "$CRL" TAGGED-CORPUS-2 DUMMY-TAGGED-CORPUS CONTEXT-RULEFILE TRAINING.LEXICON \
        > outcontext 2> errcontext \
        || die "contextual rule learning failed, see $(pwd)/errcontext"
fi
log "   $(wc -l < CONTEXT-RULEFILE) contextual rules"

# ---------------------------------------------------------------- 7. evaluation
log "7. Tagging UNTAGGED-CORPUS-2 with the final model -> UNTAGGED-CORPUS-2.tagged"
"$TAGGER" -n "$NOUN" -p "$PROPER" \
    FINAL.LEXICON UNTAGGED-CORPUS-2 BIGBIGRAMLIST LEXRULEOUTFILE CONTEXT-RULEFILE \
    > UNTAGGED-CORPUS-2.tagged

# Token accuracy against the gold standard. NB: FINAL.LEXICON is built from the
# whole corpus, so this is an optimistic (closed vocabulary) figure.
awk '
    function tag(t,   i) { i = match(t, /\/[^\/]*$/); return i ? substr(t, i + 1) : "" }
    NR == FNR { n = split($0, g); for (i = 1; i <= n; i++) gold[FNR, i] = tag(g[i]); len[FNR] = n; next }
    {
        n = split($0, t)
        if (n != len[FNR]) { skipped++; next }
        for (i = 1; i <= n; i++) { total++; if (tag(t[i]) == gold[FNR, i]) ok++ }
    }
    END {
        if (total) printf "   accuracy on TAGGED-CORPUS-2: %.2f%% (%d/%d tokens)\n", 100 * ok / total, ok, total
        if (skipped) printf "   WARNING: %d lines skipped because of token count mismatch\n", skipped
    }' TAGGED-CORPUS-2 UNTAGGED-CORPUS-2.tagged >&2

log "Done. Files for taggerXML in $(pwd):"
log "   FINAL.LEXICON BIGBIGRAMLIST LEXRULEOUTFILE CONTEXT-RULEFILE"
log "Usage: taggerXML -n $NOUN -p $PROPER FINAL.LEXICON <text> BIGBIGRAMLIST LEXRULEOUTFILE CONTEXT-RULEFILE"
