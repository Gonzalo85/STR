-- Añadir el paquete  de interrupciones de ADA
with Ada.Interrupts; use Ada.Interrupts;
-- Añadir el paquete  de nombres de interrupciones de ADA
with Ada.Interrupts.Names; use Ada.Interrupts.Names;

package controlador is
   -- Crear un objeto protegido (sin el type) de nombre ControladorSigInt

   protected ControladorSigInt is

      -- Definir un procedimiento Interrupcion sin parámetros
      procedure Interrupcion;

      -- Definir una entrada Wait sin parámetros
      entry Wait;

      -- Incluir el pragma Interrupt_Handler con Interrupcion como parámetro (indica que Interrupcion será un manejador de interrupción)
      pragma Interrupt_Handler(Interrupcion);

      -- Incluir el pragma Attach_Handler con Interrupcion y Sigint como parámetros (indicará que Interrupcion manejará la señal Sigint)
      pragma Attach_Handler(Interrupcion,SIGINT);

      -- Incluir el pragma Unreserve_All_Interrupts sin parámetros (necesario para que se puedan manejar las interrupciones)
      pragma Unreserve_All_Interrupts;

      -- Zona privada del objeto
   private

      --Definir el atributo Ocurrencia como booleano e inicializarlo a falso
      Ocurrencia : Boolean := false;

      --Fin de ControladorSigInt
   end;
end controlador;
