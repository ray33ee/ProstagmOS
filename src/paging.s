SECTION .data

SECTION .text
global	load_pd_table
global	enable_paging

;void load_pd_table(void*)
; Loads paging descriptor table into cr3 register

load_pd_table:
	
	push 	ebp
	mov	ebp, esp

	mov	eax, [esp+8] 
	mov	cr3, eax
	leave
	ret

enable_paging:
	
	push 	ebp
	mov	ebp, esp



	mov	eax, cr0
	or 	eax, 0x80000000
	mov	cr0, eax


	mov	esp, ebp
	pop	ebp
	ret
