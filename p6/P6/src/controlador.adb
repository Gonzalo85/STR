package body controlador is
   -- Implementación del cuerpo del objeto protegido ControladorSigInt
   protected body ControladorSigInt is
      -- Implementación del procedimiento Interrupcion que lo único que hace es poner Ocurrencia a verdadero (se ejecutará cuando ocurra la interrupción)
      procedure Interrupcion is begin
         Ocurrencia := True;
      end;

      -- Implementación de la entrada Wait que esperará a que Ocurrencia sea verdadera y lo único que hace es volver a ponerla a falso
      entry Wait when Ocurrencia is
      begin
         Ocurrencia := False;
      end;

   -- Fin del objeto protegido ControladorSigInt
   end ControladorSigInt;

end controlador;
