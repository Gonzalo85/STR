package body paquetebarrera is
   -- Definir el cuerpo del objeto Barrera_t
   protected body Barrera_t is
      -- Implementar el procedimiento Entrando del objeto protegido que se encargar� de incrementar el contador
      procedure Entrando is
      begin
         Cont:=Cont+1;
      end;


      -- Implementar la entrada Wait incluyendo la condici�n de paso (no han llegado a la barrera el n�mero de tareas indicado). En el c�digo, no hace nada, s�lo bloquea.
      entry Wait
        when Cont = nTareas
      is begin
         null;
      end;

      -- Implementar el procedimiento Resetear que pone el contador a 0
      procedure Resetear is
      begin
         Cont := 0;
      end;

   -- Fin del objeto protegido
      end;

   -- Implementaci�n del procedimiento SincronizarEnBarrera
   procedure SincronizarEnBarrera(Param : in out Barrera_t) is
   begin
      -- Invoca al procedimiento Entrando de la barrera
      Param.Entrando;

      -- Invoca a la entrada Wait de la barrera
      Param.Wait;

   -- Fin del procedimiento SincronizarEnBarrera
   end;

   -- Implementaci�n del procedimiento ResetearBarrera
   procedure ResetearBrrera(Param : in out Barrera_t) is
   begin

      -- Invoca al procedimiento Resetear de la barrera
      Param.Resetear;
   -- Fin del procedimiento ResetearBarrera
   end;
end paquetebarrera;
