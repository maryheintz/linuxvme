      REAL function timing(id)

      VECTOR time(6400)
      VECTOR peak(6400)

      do 10 i = 1, 6400
         call hfill(id, time(i), peak(i), 1.0)
 10   continue

      timing = 1

      end
