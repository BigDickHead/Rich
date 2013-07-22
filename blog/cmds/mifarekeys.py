mifarekeys v0.1b

Usage:
    /usr/bin/mifarekeys.py <KeyA> <KeyB>

    Create MifarePWD for access to Mifare protected memory on Dual Interface IC
    (JCOP) cards. Output is DKeyA, DKeyB and MifarePWD. DKeyA and DKeyB are used as
    the DES3 keys to generate MifarePWD with an IV of (binary) '00000000', a
    Challenge of (also binary) '00000000', and a key of DKeyA+DKeyB+DKeyA.

    Example:

    Using KeyA of A0A1A2A3A4A5 and KeyB of B0B1B2B3B4B5 should give the result:

        DKeyA:        40424446484A7E00
        DKeyB:        007E60626466686A

        MifarePWD:    8C7F46D76CE01266
