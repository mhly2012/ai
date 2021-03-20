(define row-size 100)
(define col-size 100)
(define bus-size 10)
(define addr $reg-from-cpu0)
(define size $reg-from-cpu1)
(define cpu $reg-to-cpu0)
(define sig-to-cpu $wire-to-cpu0)
INTERRUPT_0
(let row-index (div addr row-size))
(let col-index (mod addr row-size))
(read cpu row-index col-index bus-size)
(write sig-to-cpu 1)
(check-end)

