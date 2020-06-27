defmodule Sepa do
  @moduledoc """
  Documentation for `Sepa`.
  """
  alias Sepa.CreditTransferInitiation, as: CTI

  @type iban :: String.t()

  @type bic :: String.t()

  @doc """
  ## Examples

      iex> cti =
      ...>   %Sepa.CreditTransferInitiation{
      ...>     name: "Max Mustermann",
      ...>     iban: "DE22380200904030000000",
      ...>     bic: "HYVEDEMM402"
      ...>   }
      iex> match?({:ok, _}, Sepa.to_xml(cti))
      true

  """
  def to_xml(%CTI{} = cti) do
    with {:ok, sepa} <- Sepa.Nif.create_message(CTI.type()),
         :ok <- Sepa.Nif.set_name(sepa, String.to_charlist(cti.name)),
         :ok <- Sepa.Nif.set_iban(sepa, String.to_charlist(cti.iban)),
         :ok <- Sepa.Nif.set_bic(sepa, String.to_charlist(cti.bic)),
         :ok <- add(sepa, cti.transactions),
         {:ok, xml} <- Sepa.Nif.to_xml(sepa) do
      {:ok, List.to_string(xml)}
    end
  end

  defp add(sepa, transactions) do
    Enum.reduce_while(transactions, :ok, fn transaction, acc ->
      transaction =
        transaction
        |> Enum.map(fn
          {:amount, v} -> {'amount', String.to_charlist(Decimal.to_string(v))}
          {k, v} -> {String.to_charlist("#{k}"), String.to_charlist(v)}
        end)
        |> Map.new()

      case Sepa.Nif.add(sepa, transaction) do
        :ok -> {:cont, acc}
        {:error, err} -> {:halt, {:error, List.to_string(err)}}
      end
    end)
  end

  @doc """
  ## Examples

      iex> Sepa.check_iban("DE22380200904030000000")
      true
      iex> Sepa.check_iban("DE22380200904030000001")
      false

  """
  def check_iban(iban) do
    Sepa.Nif.check_iban(String.to_charlist(iban))
  end

  @doc """
  ## Examples

      iex> Sepa.get_bank_name("BYLADEM1ERH")
      {:ok, "STADT-UND KREISSPARKASSE ERLANGEN HOECHSTADT HERZOGENAURACH"}
      iex> Sepa.get_bank_name("BYLADAM1ERH")
      :error

  """
  def get_bank_name(bic) do
    with {:ok, bank_name} <- Sepa.Nif.bic_get_bank_name(String.to_charlist(bic)) do
      {:ok, List.to_string(bank_name)}
    end
  end

  @doc """
  ## Examples

      iex> Sepa.convert_iban("DE", "7325022", "26580070")
      {:ok, "DE32265800700732502200", 1}
      iex> Sepa.convert_iban("DE", "40033086", "30020500")
      {:ok, "DE02500202000040033086", 15}
      iex> Sepa.convert_iban("DE", "123", "123")
      :error

  """
  def convert_iban(country, accounts, bank_id) do
    with {:ok, iban, status} <-
           Sepa.Nif.iban_convert(
             String.to_charlist(country),
             String.to_charlist(accounts),
             String.to_charlist(bank_id)
           ) do
      {:ok, List.to_string(iban), status}
    end
  end
end
