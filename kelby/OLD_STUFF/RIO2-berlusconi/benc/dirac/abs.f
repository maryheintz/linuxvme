      REAL function abs(num, id)

      VECTOR dist(6)
      VECTOR rate(6)

      do 10 i = 1, 6
         call hfill(id, dist(i), 1.5*rate(i), 1.0)
         call hfill(id, dist(i), 0.5*rate(i), 1.0)
 10   continue

      abs = 1.

      end
