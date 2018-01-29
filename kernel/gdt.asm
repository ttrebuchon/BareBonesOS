[bits 32]

gdtr    DW 0
        DD 0


global loadGdt
loadGdt:
    mov     eax, [esp + 4]
    mov     [gdtr + 2], eax
    mov     ax, [esp + 8]
    mov     [gdtr], ax
    lgdt    [gdtr]
    ret



global reloadGdtSegments
reloadGdtSegments:

    jmp 0x08:.reload_CS

.reload_CS:
    MOV AX, 0x10
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX
    MOV SS, AX
    RET