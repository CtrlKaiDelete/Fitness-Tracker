# MEFI 1.0

MEFI is the Micro Extensible Firmware Interface used in Microkernel (Yes, a UEFI clone).

It passes a pointer to `MEFI::SystemTable` in `r0` when it enters the
kernel reset handler. All public functions (should) follow AAPCS, as all tables are 
aligned to four bytes, and every table begins with a signature, revision, and size so
later revisions can extend the ABI without changing existing fields.