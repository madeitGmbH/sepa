defmodule Sepa.CreditTransferInitiation do
  @typedoc "Name des Zahlungsempfängers. Maximal 70 Zeichen."
  @type name :: String.t()

  @typedoc """
  Sogenannter „end-to-end identifier“, eine frei wählbare ID welche diese Transaktion eindeutig identifiziert und u.a. auf dem Kontoauszug des Zahlungsempfängers ausgegeben wird. Wird keine ID angegeben, wird standardmäßig NOTPROVIDED verwendet. Maximal 35 Zeichen (A-Za-z0-9 +?/-:().,').
  """
  @type transaction_id :: String.t()

  @typedoc "Betrag in Euro (max. zwei Nachkommastellen)"
  @type amount :: Decimal.t()

  @type t :: %__MODULE__{
          iban: Sepa.iban(),
          bic: Sepa.bic(),
          name: String.t(),
          batch_booking: boolean,
          transactions: [
            %{
              required(:name) => name,
              required(:amount) => amount,
              required(:iban) => Sepa.iban(),
              required(:bic) => Sepa.bic(),
              optional(:ref) => String.t(),
              optional(:purp) => String.t(),
              optional(:addr_line_1) => String.t(),
              optional(:addr_line_2) => String.t(),
              optional(:cc) => String.t(),
              optional(:id) => transaction_id
            }
          ]
        }

  defstruct iban: nil,
            bic: nil,
            name: nil,
            batch_booking: true,
            transactions: []

  @doc false
  def type, do: 2
end
